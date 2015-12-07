#include "lordaeron/interactive/light_mesh.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/scene/scene_node.h"


namespace lord {
using namespace azer;
const float kControllerObjectAlpha = 0.8f;

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
  local_transform_ = Matrix4::kIdentity;
}

void LightMesh::Render(Renderer* renderer) {
  ScopedCullingMode auto_culling(kCullNone, renderer);
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
  InitPickedMesh();
  local_transform_ = std::move(RotateX(Degree(90.0f)));
}

void SpotLightControllerMesh::InitMesh() {
  float kSpotHeight = 0.7f;
  float kSpotRadius = 0.3f;
  float kBaseHeight = 0.3f;
  float kBaseRadius = 0.1f;

  Context* context = Context::instance();
  RenderSystem* rs = RenderSystem::Current();
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
    Matrix4 trans = std::move(Translate(0.0f, -(kSpotHeight - 0.5f), 0.0f)) 
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
    Matrix4 trans = std::move(Translate(0.0f, -0.5f, 0.0f)) 
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
  DCHECK(node_->type() == kLampSceneNode);
  Context* ctx = Context::instance();
  BlendingPtr blending = ctx->GetDefaultBlending();
  effect_ = CreateDiffuseEffect();
  Light* light = node_->mutable_data()->light();
  const SpotLight& spot = light->spot_light();
  float range = spot.range;
  
  float top_radius = 0.2f;
  float inner_sine = std::sqrt(1 - spot.theta * spot.theta);
  float inner_radius = range * inner_sine / spot.theta;
  float outer_sine = std::sqrt(1 - spot.phi * spot.phi);
  float outer_radius = range * outer_sine / spot.phi;
  GeometryObjectPtr obj1 = new CylinderObject(
      effect_->GetVertexDesc(), inner_radius, top_radius, range);
  MeshPartPtr inner_cone = obj1->CreateObject(effect_.get());
  inner_cone->SetBlending(blending.get());
  GeometryObjectPtr obj2 = new CylinderObject(
      effect_->GetVertexDesc(), outer_radius, top_radius, range);
  MeshPartPtr outer_cone = obj2->CreateObject(effect_.get());
  outer_cone->SetBlending(blending.get());
  picked_part_.push_back(inner_cone);
  picked_part_.push_back(outer_cone);
}


// class DirLightControllerMesh
DirLightControllerMesh::DirLightControllerMesh(SceneNode* node, 
                                                 DiffuseEffect* effect) 
    : LightMesh(node, effect) {
  local_transform_ = std::move(RotateX(Degree(90.0f)));
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
  {
    // create VertexData
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitConeVertexData(32, desc);
    IndicesDataPtr idata = InitConeIndicesData(32);
    VertexPack vpack(vdata.get());
    Matrix4 trans = std::move(Translate(0.0f, kConeY - 0.5f, 0.0f)) 
        * std::move(Scale(kConeRadius, kConeHeight, kConeRadius));
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
        1.0f, 1.0f, 1.0f, kStack, kSlice, false, desc);
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice, false);
    VertexPack vpack(vdata.get());
    Matrix4 trans = std::move(Translate(0.0f, -0.5, 0.0f)) 
        * std::move(Scale(kCylinderRadius, kConeY, kCylinderRadius));
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

void DirLightControllerMesh::InitPickedMesh() {

}

LightMeshPtr CreateLightMesh(SceneNode* node) {
  DCHECK(node->type() == kLampSceneNode);
  Light* light = node->mutable_data()->light();
  DiffuseEffectPtr effect = CreateDiffuseEffect();
  DCHECK(effect.get());
  switch (light->type()) {
    case kPointLight: 
      return LightMeshPtr(new PointLightControllerMesh(node, effect.get()));
    case kSpotLight:
      return LightMeshPtr(new SpotLightControllerMesh(node, effect.get()));
    case kDirectionalLight:
      return LightMeshPtr(new DirLightControllerMesh(node, effect.get()));
    default:
      NOTREACHED();
      return LightMeshPtr();
  }
}

// class LightMeshProvider
LightMeshProvider::LightMeshProvider(SceneNode* node, const Matrix4& ltran) 
    : node_(node),
      light_(node->mutable_data()->light()),
      local_transform_(ltran) {
}

LightMeshProvider::~LightMeshProvider() {}
void LightMeshProvider::UpdateParams(const FrameArgs& args) {
  world_ = std::move(GenWorldMatrixForSceneNode(node_) * local_transform_);
}

const Vector4& LightMeshProvider::color() const {
  return light_->diffuse();
}

const Vector4& LightMeshProvider::emission() const {
  return light_->diffuse();
}

const azer::Matrix4& LightMeshProvider::world() const {
  return world_;
}

// class LightMeshDiffuseEffectAdapter
LightMeshDiffuseEffectAdapter::LightMeshDiffuseEffectAdapter() {
}

EffectAdapterKey LightMeshDiffuseEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(LightMeshProvider).name());
}

void LightMeshDiffuseEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(effect);
    const LightMeshProvider* provider =
        dynamic_cast<const LightMeshProvider*>(params);
    DCHECK(provider);
    Vector4 color = provider->color();
    color.w = 0.3f;
    effect->SetColor(color);
    effect->SetEmission(provider->emission());
    effect->SetWorld(provider->world());
}
}  // namespace lord
