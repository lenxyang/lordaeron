#include "lordaeron/interactive/light_controller.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "lordaeron/env.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/scene_node.h"


namespace lord {
using namespace azer;
const float kControllerObjectAlpha = 0.8f;

namespace {
void GenerateCrossLine(float radius, float y, VertexPack* vpack) {
  VertexPos npos;
  CHECK(GetSemanticIndex("normal", 0, vpack->desc(), &npos));
  Vector4 normal(0.0f, 1.0f, 0.0f, 0.0f);

  vpack->WriteVector4(Vector4(-radius, y, 0.0f, 1.0f), VertexPos(0, 0));
  vpack->WriteVector4(normal, npos);
  vpack->next(1);
  vpack->WriteVector4(Vector4( radius, y, 0.0f, 1.0f), VertexPos(0, 0));
  vpack->WriteVector4(normal, npos);
  vpack->next(1);
  vpack->WriteVector4(Vector4(0.0f, y, -radius, 1.0f), VertexPos(0, 0));
  vpack->WriteVector4(normal, npos);
  vpack->next(1);
  vpack->WriteVector4(Vector4(0.0f, y,  radius, 1.0f), VertexPos(0, 0));
  vpack->WriteVector4(normal, npos);
  vpack->next(1);
}

void GenerateCircle(float radius, float y, int32 slice, VertexPack* vpack) {
  VertexPos npos;
  CHECK(GetSemanticIndex("normal", 0, vpack->desc(), &npos));
  float degree = 360.0f / (float)slice;
  Vector4 normal(0.0f, 1.0f, 0.0f, 0.0f);
  for (int32 i = 0; i < slice + 1; ++i) {
    int index = i % slice;
    float x = cos(Degree(i * degree)) * radius;
    float z = sin(Degree(i * degree)) * radius;
    Vector4 pos(x, y, z, 1.0f);
    for (int j = 0; j < 2; ++j) {
      vpack->WriteVector4(pos, VertexPos(0, 0));
      vpack->WriteVector4(normal, npos);
      vpack->next(1);
      if (i == 0 || i == slice)
        break;
    }
  }
}

void TransformVertex(const Matrix4& trans, SlotVertexData* vdata,
                     Vector3* vmin, Vector3* vmax) {
  VertexDesc* desc = vdata->vertex_desc();
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

// LightControllerProvider
LightControllerProvider::LightControllerProvider(RenderNode* node)
    : node_(node) {
  Light* light = node->GetSceneNode()->mutable_data()->light();
  emission_ = light->diffuse() * 0.5f;
  color_ = light->diffuse();
  local_transform_ = Matrix4::kIdentity;
}

void LightControllerProvider::Update() {
  world_ = node_->GetWorld() * local_transform_;
}

void LightControllerProvider::SetLocalTransform(const Matrix4& local) {
  local_transform_ = local;
}

const Matrix4& LightControllerProvider::GetPV() const {
  return node_->GetPV();
}

// class LightController
LightController::LightController(RenderNode* node)
    : node_(node) {
  DCHECK(node->GetSceneNode()->type() == kLampSceneNode);
  provider_ = new LightControllerProvider(node);
}

void LightController::Update(const FrameArgs& args) {
  provider_->Update();
}

void LightController::Render(Renderer* renderer) {
  light_mesh_->Render(renderer);
}

// class PointLightController
PointLightController::PointLightController(RenderNode* node)
    : LightController(node) {
  LordEnv* ctx = LordEnv::instance();
  effect_ = CreateDiffuseEffect();
  light_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  controller_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  InitMesh();
  InitControllerMesh();
  light_mesh_->AddProvider(provider_);
  controller_mesh_->AddProvider(provider_);
}

void PointLightController::InitMesh() {
  GeometryObjectPtr obj = new SphereObject(effect_->vertex_desc(), 0.1f);
  MeshPartPtr part = obj->CreateObject(effect_.get());
  light_mesh_->AddMeshPart(part.get());
}

void PointLightController::InitControllerMesh() {
  SceneNode* scene_node = node_->GetSceneNode();
  DCHECK(scene_node->type() == kLampSceneNode);
  Light* light = scene_node->mutable_data()->light();
  float range = light->point_light().atten.range;
  LordEnv* ctx = LordEnv::instance();
  BlendingPtr blending = ctx->GetDefaultBlending();
  GeometryObjectPtr obj = new SphereObject(effect_->vertex_desc(), range);
  MeshPartPtr part = obj->CreateObject(effect_.get());
  part->SetBlending(blending.get());
  controller_mesh_->AddMeshPart(part);
}

void PointLightController::Update(const FrameArgs& args) {
  LightController::Update(args);
}

void PointLightController::Render(Renderer* renderer) {
  LightController::Render(renderer);
  if (node_->GetSceneNode()->picked()) {
    controller_mesh_->Render(renderer);
  }
}

// class SpotLightController
const float SpotLightController::kTopRadius = 0.2f;
SpotLightController::SpotLightController(RenderNode* node)
    : LightController(node) {
  rasterizer_state_ = RenderSystem::Current()->CreateRasterizerState();
  rasterizer_state_->SetCullingMode(kCullNone);
  
  LordEnv* ctx = LordEnv::instance();
  provider_->SetLocalTransform(std::move(RotateX(Degree(-90.0f))));
  effect_ = CreateDiffuseEffect();
  light_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  controller_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  line_mesh_ = new Mesh(ctx->GetEffectAdapterContext());

  SceneNode* scene_node = node_->GetSceneNode();
  Light* light = scene_node->mutable_data()->light();
  InitMesh();
  InitControllerMesh();
  // CreateCrossCircle(10.0f, light);
  CreateCrossCircle(light->spot_light().range, light);
  CreateBorderLine(light);

  light_mesh_->AddProvider(provider_);
  controller_mesh_->AddProvider(provider_);
  line_mesh_->AddProvider(provider_);
}

void SpotLightController::InitMesh() {
  float kSpotHeight = 0.7f;
  float kSpotRadius = 0.3f;
  float kBaseHeight = 0.3f;
  float kBaseRadius = 0.1f;

  RenderSystem* rs = RenderSystem::Current();
  VertexDesc* desc = effect_->vertex_desc();
  // spot cylinder
  {
    // create VertexData
    const int32 kStack = 10, kSlice = 32;
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitCylinderVertexData(
        kSpotRadius, kBaseRadius, kSpotHeight, kStack, kSlice, false, desc);
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice, false);
    VertexPack vpack(vdata.get());
    Matrix4 trans = std::move(Translate(0.0f, -(kSpotHeight - 0.5f), 0.0f));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect_.get());
    EntityPtr entity(new Entity(effect_->vertex_desc(), vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    light_mesh_->AddMeshPart(part);
  }

  {
    // create VertexData
    const int32 kStack = 10, kSlice = 32;
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitCylinderVertexData(
        kBaseRadius, kBaseRadius, kBaseHeight, kStack, kSlice, false, desc);
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice, false);
    VertexPack vpack(vdata.get());
    Matrix4 trans = std::move(Translate(0.0f, -0.5f, 0.0f));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect_.get());
    EntityPtr entity(new Entity(effect_->vertex_desc(), vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    light_mesh_->AddMeshPart(part);
  }
}

void SpotLightController::InitControllerMesh() {
  LordEnv* ctx = LordEnv::instance();
  SceneNode* scene_node = node_->GetSceneNode();
  Light* light = scene_node->mutable_data()->light();
  DCHECK(scene_node->type() == kLampSceneNode);
  BlendingPtr blending = ctx->GetDefaultBlending();
  const SpotLight& spot = light->spot_light();
  float range = spot.range;
  
  float top_radius = kTopRadius;
  float inner_sine = std::sqrt(1 - spot.theta * spot.theta);
  float inner_radius = range * inner_sine / spot.theta;
  float outer_sine = std::sqrt(1 - spot.phi * spot.phi);
  float outer_radius = range * outer_sine / spot.phi;
  GeometryObjectPtr obj1 = new CylinderObject(
      effect_->vertex_desc(), inner_radius, top_radius, range, 64, 64, false);
  MeshPartPtr inner_cone = obj1->CreateObject(effect_.get());
  inner_cone->SetBlending(blending.get());
  GeometryObjectPtr obj2 = new CylinderObject(
      effect_->vertex_desc(), outer_radius, top_radius, range, 64, 64, false);
  MeshPartPtr outer_cone = obj2->CreateObject(effect_.get());
  outer_cone->SetBlending(blending.get());
  controller_mesh_->AddMeshPart(inner_cone);
  controller_mesh_->AddMeshPart(outer_cone);
}

void SpotLightController::Update(const FrameArgs& args) {
  LightController::Update(args);
}

void SpotLightController::Render(Renderer* renderer) {
  {
    ScopedRasterizerState scoped_cull(renderer, rasterizer_state_);
    LightController::Render(renderer);
  }

  if (node_->GetSceneNode()->picked()) {
    line_mesh_->Render(renderer);
    controller_mesh_->Render(renderer);
  }
}

void SpotLightController::CreateCrossCircle(float mid, Light* light) {
  RenderSystem* rs = RenderSystem::Current();
  const SpotLight& spot = light->spot_light();
  float range = spot.range;
  float top_radius = kTopRadius;
  
  float inner_sine = std::sqrt(1 - spot.theta * spot.theta);
  float inner_radius = range * inner_sine / spot.theta;
  float outer_sine = std::sqrt(1 - spot.phi * spot.phi);
  float outer_radius = range * outer_sine / spot.phi;
  float inner_top = kTopRadius * spot.theta / inner_sine;
  float outer_top = kTopRadius * spot.phi / outer_sine;
  float mid_inner = kTopRadius * (mid + inner_top) / inner_top;
  float mid_outer = kTopRadius * (mid + outer_top) / outer_top;
  VertexDesc* desc = effect_->vertex_desc();
  float y = range - mid;

  int32 kSlice = 64;
  int32 kVertexNum = kSlice * 4 + 4;
  SlotVertexDataPtr vdata(new SlotVertexData(desc, kVertexNum));
  VertexPack vpack(vdata.get());
  vpack.first();
  GenerateCircle(mid_inner, mid, kSlice, &vpack);
  GenerateCircle(mid_outer, mid, kSlice, &vpack);
  GenerateCrossLine(mid_outer, mid, &vpack);
  EntityPtr entity(new Entity(effect_->vertex_desc()));
  entity->SetVertexBuffer(rs->CreateVertexBuffer(VertexBuffer::Options(), vdata), 0);
  entity->set_topology(kLineList);
  MeshPartPtr part(new MeshPart(effect_.get()));
  part->AddEntity(entity);
  line_mesh_->AddMeshPart(part);
}

void SpotLightController::CreateBorderLine(Light* light) {
  RenderSystem* rs = RenderSystem::Current();
  const SpotLight& spot = light->spot_light();
  float range = spot.range;
  float top_radius = kTopRadius;
  float outer_sine = std::sqrt(1 - spot.phi * spot.phi);
  float outer_radius = range * outer_sine / spot.phi;
  VertexPos npos;
  Vector4 normal(0.0f, 1.0f, 0.0f, 0.0f);
  SlotVertexDataPtr vdata(new SlotVertexData(effect_->vertex_desc(), 8));
  VertexPack vpack(vdata.get());
  CHECK(GetSemanticIndex("normal", 0, effect_->vertex_desc(), &npos));
  vpack.first();

  Vector4 pos[8] = {
    Vector4(-top_radius, 0.0f, 0.0f, 1.0f),
    Vector4(-outer_radius, spot.range, 0.0f, 1.0f),
    Vector4( top_radius, 0.0f, 0.0f, 1.0f),
    Vector4( outer_radius, spot.range, 0.0f, 1.0f),
    Vector4(0.0f, 0.0f,       -top_radius,   1.0f),
    Vector4(0.0f, spot.range, -outer_radius, 1.0f),
    Vector4(0.0f, 0.0f,        top_radius,   1.0f),
    Vector4(0.0f, spot.range,  outer_radius, 1.0f),
  };
  for (uint32 i = 0; i < arraysize(pos); ++i) {
    vpack.WriteVector4(pos[i], VertexPos(0, 0));
    vpack.WriteVector4(normal, npos);
    vpack.next(1);
  }

  EntityPtr entity(new Entity(effect_->vertex_desc()));
  entity->SetVertexBuffer(rs->CreateVertexBuffer(VertexBuffer::Options(), vdata), 0);
  entity->set_topology(kLineList);
  MeshPartPtr part(new MeshPart(effect_.get()));
  part->AddEntity(entity);
  line_mesh_->AddMeshPart(part);
}

// class DirLightController
DirLightController::DirLightController(RenderNode* node)
    : LightController(node) {
  LordEnv* ctx = LordEnv::instance();
  effect_ = CreateDiffuseEffect();
  provider_->SetLocalTransform(std::move(RotateX(Degree(-90.0f))));
  light_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  InitMesh();
  InitControllerMesh();
  light_mesh_->AddProvider(provider_);
}

void DirLightController::InitMesh() {
  const float kConeHeight = 0.3f;
  const float kConeRadius = 0.2f;
  const float kConeY = 0.8f;
  const float kCylinderRadius = 0.08f;
  RenderSystem* rs = RenderSystem::Current();
  VertexDesc* desc = effect_->vertex_desc();
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
    EntityPtr entity(new Entity(effect_->vertex_desc(), vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    light_mesh_->AddMeshPart(part);
  }

  {
    // create VertexData
    const int32 kStack = 10, kSlice = 32;
    Vector3 vmin(99999.0f, 99999.0f, 99999.0f);
    Vector3 vmax(-99999.0f, -99999.0f, -99999.0f);
    SlotVertexDataPtr vdata = InitCylinderVertexData(
        kCylinderRadius, kCylinderRadius, kConeY, kStack, kSlice, false, desc);
    IndicesDataPtr idata = InitCylinderIndicesData(kStack, kSlice, false);
    VertexPack vpack(vdata.get());
    Matrix4 trans = std::move(Translate(0.0f, -0.5, 0.0f));
    TransformVertex(trans, vdata.get(), &vmin, &vmax);
    VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
    IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
    MeshPartPtr part = new MeshPart(effect_.get());
    EntityPtr entity(new Entity(effect_->vertex_desc(), vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    light_mesh_->AddMeshPart(part);
  }
}

void DirLightController::InitControllerMesh() {
}

void DirLightController::Update(const FrameArgs& args) {
  LightController::Update(args);
}

void DirLightController::Render(Renderer* renderer) {
  LightController::Render(renderer);
}

// class LightControllerEffectAdapter
LightControllerEffectAdapter::LightControllerEffectAdapter() {}
const EffectAdapterKey LightControllerEffectAdapter::kAdapterKey =
    std::make_pair(typeid(DiffuseEffect).name(),
                   typeid(LightControllerProvider).name());

void LightControllerEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  LordEnv* ctx = LordEnv::instance();
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  const LightControllerProvider* provider = 
      dynamic_cast<const LightControllerProvider*>(params);
  DCHECK(provider && effect);
  Vector4 color = provider->color();
  color.w = 0.3f;
  effect->SetColor(color);
  effect->SetEmission(provider->emission());
  effect->SetWorld(provider->GetWorld());
  effect->SetPV(provider->GetPV());
  effect->SetDirLight(ctx->GetInternalLight());
  effect->SetColor(provider->color());
}
}  // namespace lord
