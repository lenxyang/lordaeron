#include "lordaeron/interactive/light_mesh.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/scene/scene_node.h"


namespace lord {
using namespace azer;

namespace {
void TransformVertex(const Matrix4& trans, SlotVertexData* vdata,
                     Vector3* vmin, Vector3* vmax) {
  VertexDesc* desc = vdata->desc();
  VertexPos posidx(0, 0);
  VertexPos normalidx;
  bool kHasNormal0Idx = GetSemanticIndex("normal", 0, desc, &normalidx);
  Vector4 pos, normal;
  VertexPack vpack(vdata);
  vpack.first();
  while (!vpack.end()) {
    vpack.ReadVector4(&pos, posidx);
    pos = trans * pos;
    vpack.WriteVector4(pos, posidx);
    UpdateVMinAndVMax(Vector3(pos.x, pos.y, pos.z), vmin, vmax);
    if (kHasNormal0Idx) {
      vpack.ReadVector4(&normal, normalidx);
      normal = trans * normal;
      normal.Normalize();
      vpack.WriteVector4(normal, normalidx);
    }

    vpack.next(1);
  }
}
}  // namespace


LightMesh::LightMesh(SceneNode* node, DiffuseEffect* effect) 
    : Mesh(Context::instance()->GetEffectAdapterContext()),
      node_(node),
      effect_(effect) {
}

void LightMesh::Render(Renderer* renderer) {
  Mesh::Render(renderer);
  if (node_->picked()) {
    RenderPickedPart(renderer);
  }
}

void LightMesh::RenderPickedPart(Renderer* renderer) {
  for (auto iter = picked_part_.begin(); iter != picked_part_.end(); ++iter) {
    MeshPart* part = iter->get();
    ApplyParams(part->effect());
    part->Render(renderer);
  }
}

PointLightControllerMesh::PointLightControllerMesh(
    SceneNode* node, DiffuseEffect* effect) 
    : LightMesh(node, effect) {
  Context* ctx = Context::instance();
  GeometryObjectPtr obj = new SphereObject(effect->GetVertexDesc(), 0.1f);
  MeshPartPtr part = obj->CreateObject(effect_.get());
  AddMeshPart(part.get());
  InitPickedMesh();
}

void PointLightControllerMesh::InitPickedMesh() {
  DCHECK(node_->type() == kLampSceneNode);
  Light* light = node_->mutable_data()->light();
  float range = light->point_light().atten.range;
  Context* ctx = Context::instance();
  BlendingPtr blending = ctx->GetDefaultBlending();
  GeometryObjectPtr obj = new SphereObject(effect_->GetVertexDesc(), range);
  MeshPartPtr part = obj->CreateObject(effect_.get());
  part->SetBlending(blending.get());
  picked_part_.push_back(part);
}

// class SpotLightControllerMesh
SpotLightControllerMesh::SpotLightControllerMesh(SceneNode* node, 
                                                 DiffuseEffect* effect) 
    : LightMesh(node, effect) {
  InitMesh();
}

void SpotLightControllerMesh::InitMesh() {
  float kSpotHeight = 0.7f;
  float kSpotRadius = 0.3f;
  float kBaseHeight = 0.3f;
  float kBaseRadius = 0.1f;

  Context* context = Context::instance();
  RenderSystem* rs = RenderSystem::Current();
  Matrix4 rotation = std::move(RotateX(Degree(90.0f)));
  VertexDesc* desc = effect_->GetVertexDesc();
  // spot cylinder
  {
    // create VertexData
    const int32 kStack = 10, kSlice = 32;
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitCylinderVertexData(
        kSpotRadius, kBaseRadius, 1.0f, kStack, kSlice, false, desc);
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice, false);
    VertexPack vpack(vdata.get());
    Matrix4 trans = rotation 
        * std::move(Translate(0.0f, -(kSpotHeight - 0.5f), 0.0f)) 
        * std::move(Scale(1.0f, kSpotHeight, 1.0f));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect_.get());
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    AddMeshPart(part);
  }

  {
    // create VertexData
    const int32 kStack = 10, kSlice = 32;
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitCylinderVertexData(
        kBaseRadius, kBaseRadius, 1.0f, kStack, kSlice, false, desc);
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice, false);
    VertexPack vpack(vdata.get());
    Matrix4 trans = rotation * std::move(Translate(0.0f, -0.5f, 0.0f)) 
        * std::move(Scale(1.0f, kBaseHeight, 1.0f));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect_.get());
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    AddMeshPart(part);
  }
}

void SpotLightControllerMesh::InitPickedMesh() {

}


// class DirLightControllerMesh
DirLightControllerMesh::DirLightControllerMesh(SceneNode* node, 
                                                 DiffuseEffect* effect) 
    : LightMesh(node, effect) {
  InitMesh();
}

void DirLightControllerMesh::InitMesh() {
  const float kConeHeight = 0.3f;
  const float kConeRadius = 0.2f;
  const float kConeY = 0.8f;
  const float kCylinderRadius = 0.08f;
  Context* context = Context::instance();
  RenderSystem* rs = RenderSystem::Current();
  VertexDesc* desc = effect_->GetVertexDesc();
  Matrix4 rotation = std::move(RotateX(Degree(90.0f)));
  {
    // create VertexData
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitConeVertexData(32, desc);
    IndicesDataPtr idata = InitConeIndicesData(32);
    VertexPack vpack(vdata.get());
    Matrix4 trans = rotation
        * std::move(Translate(0.0f, kConeY - 0.5f, 0.0f)) 
        * std::move(Scale(kConeRadius, kConeHeight, kConeRadius));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect_.get());
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
  }

  {
    // create VertexData
    const int32 kStack = 10, kSlice = 32;
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitCylinderVertexData(
        1.0f, 1.0f, 1.0f, kStack, kSlice, false, desc);
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice, false);
    VertexPack vpack(vdata.get());
    Matrix4 trans = rotation
        * std::move(Translate(0.0f, -0.5, 0.0f)) 
        * std::move(Scale(kCylinderRadius, kConeY, kCylinderRadius));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect_.get());
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
  }
}

void DirLightControllerMesh::InitPickedMesh() {

}

MeshPtr CreateLightMesh(SceneNode* node) {
  DCHECK(node->type() == kLampSceneNode);
  Light* light = node->mutable_data()->light();
  DiffuseEffectPtr effect = CreateDiffuseEffect();
  DCHECK(effect.get());
  switch (light->type()) {
    case kPointLight: 
      return MeshPtr(new PointLightControllerMesh(node, effect.get()));
    default:
      return light->GetLightMesh();
  }
}
}  // namespace lord
