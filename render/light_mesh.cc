#include "lordaeron/render/light_mesh.h"

#include "azer/render/render.h"
#include "lordaeron/context.h"
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

MeshPtr CreatePointLightMesh() {
  Context* context = Context::instance();
  MeshPtr mesh = new Mesh;
  DiffuseEffectPtr effect = CreateDiffuseEffect();
  GeometryObjectPtr obj = new SphereObject(effect->GetVertexDesc());
  MeshPartPtr part = obj->CreateObject(effect.get());
  mesh->AddMeshPart(part.get());
  mesh->SetEffectAdapterContext(context->GetEffectAdapterContext());
  return mesh;
}

MeshPtr CreateSpotLightMesh() {
  float kSpotHeight = 0.7f;
  float kSpotRadius = 0.3f;
  float kBaseRaidus = 0.1f;
  float kBaseHeight = 0.3f;

  Context* context = Context::instance();
  RenderSystem* rs = RenderSystem::Current();
  MeshPtr mesh = new Mesh;
  DiffuseEffectPtr effect = CreateDiffuseEffect();
  Matrix4 rotation = std::move(RotateX(Degree(-90.0f)));
  // spot cylinder
  {
    // create VertexData
    const int32 kStack = 10, kSlice = 32;
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitCylinderVertexData(
        kSpotRadius, kBaseRadius, kStack, kSlice, effect->GetVertexDesc());
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice);
    VertexPack vpack(vdata.get());
    Matrix4 trans = std::move(RotateX(Degree(-90.0f)))
        * std::move(Translate(0.0f, -(kSpotHeight - 0.5f), 0.0f)) 
        * std::move(Scale(1.0f, kSpotHeight, 1.0f));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect.get());
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    mesh->AddMeshPart(part);
  }

  {
    // create VertexData
    const int32 kStack = 10, kSlice = 32;
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitCylinderVertexData(
        kSpotRadius, kBaseRadius, kStack, kSlice, effect->GetVertexDesc());
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice);
    VertexPack vpack(vdata.get());
    Matrix4 trans = std::move(RotateX(Degree(-90.0f)))
        * std::move(Translate(0.0f, -0.5f., 0.0f)) 
        * std::move(Scale(1.0f, kBaseHeight, 1.0f));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect.get());
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    mesh->AddMeshPart(part);
  }

  mesh->SetEffectAdapterContext(context->GetEffectAdapterContext());
  return mesh;
}

MeshPtr CreateDirectionalLightMesh() {
  const float kConeHeight = 0.3f;
  const float kConeRadius = 0.2f;
  const float kConeY = 0.8f;
  const float kCylinderRadius = 0.08f;
  Context* context = Context::instance();
  RenderSystem* rs = RenderSystem::Current();
  MeshPtr mesh = new Mesh;
  DiffuseEffectPtr effect = CreateDiffuseEffect();

  Matrix4 rotation = std::move(RotateX(Degree(-90.0f)));
  {
    // create VertexData
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitConeVertexData(32, effect->GetVertexDesc());
    IndicesDataPtr idata = InitConeIndicesData(32);
    VertexPack vpack(vdata.get());
    Matrix4 trans = std::move(RotateX(Degree(-90.0f)))
        * std::move(Translate(0.0f, kConeY - 0.5f, 0.0f)) 
        * std::move(Scale(kConeRadius, kConeHeight, kConeRadius));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect.get());
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    mesh->AddMeshPart(part);
  }

  {
    // create VertexData
    const int32 kStack = 10, kSlice = 32;
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitCylinderVertexData(1.0f, 1.0f, kStack, kSlice, 
                                                     effect->GetVertexDesc());
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice);
    VertexPack vpack(vdata.get());
    Matrix4 trans = std::move(RotateX(Degree(-90.0f)))
        * std::move(Translate(0.0f, -0.5, 0.0f)) 
        * std::move(Scale(kCylinderRadius, kConeY, kCylinderRadius));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect.get());
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    mesh->AddMeshPart(part);
  }

  mesh->SetEffectAdapterContext(context->GetEffectAdapterContext());
  return mesh;
}

LightColorProvider::LightColorProvider(Light* light) 
    : light_(light) {
}

LightColorProvider::~LightColorProvider() {
}

void LightColorProvider::UpdateParams(const FrameArgs& args) {
}

const Vector4& LightColorProvider::color() const { 
  return light_->ambient();
}

// class LightColorDiffuseEffectAdapter
LightColorDiffuseEffectAdapter::LightColorDiffuseEffectAdapter() {
}

EffectAdapterKey LightColorDiffuseEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(LightColorProvider).name());
}

void LightColorDiffuseEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(effect);
  const LightColorProvider* provider = dynamic_cast<const LightColorProvider*>(params);
  DCHECK(provider);
  effect->SetColor(provider->color() * 3);
}
}  // namespace lord
