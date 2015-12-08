#include "lordaeron/interactive/light_controller.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_render_tree.h"


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

LightControllerProvider::LightControllerProvider(SceneRenderNode* node) 
    : node_(node) {
  Light* light = node->GetSceneNode()->mutable_data()->light();
  emission_ = light->diffuse() * 0.5f;
  color_ = light->diffuse();
  local_transform_ = Matrix4::kIdentity;
}

const azer::Matrix4& LightControllerProvider::GetPV() const {
  return node_->GetPV();
}

void LightControllerProvider::UpdateParams(const azer::FrameArgs& args) {
  world_ = node_->GetWorld() * local_transform_;
}

void LightControllerProvider::SetLocalTransform(const azer::Matrix4& local) {
  local_transform_ = local;
}


// class LightControllerEffectAdapter
LightControllerEffectAdapter::LightControllerEffectAdapter() {
}

EffectAdapterKey LightControllerEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(LightControllerProvider).name());
}

void LightControllerEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  Context* ctx = Context::instance();
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(effect);
  const LightControllerProvider* provider =
      dynamic_cast<const LightControllerProvider*>(params);
  DCHECK(provider);
  Vector4 color = provider->color();
  color.w = 0.3f;
  effect->SetColor(color);
  effect->SetEmission(provider->emission());
  effect->SetWorld(provider->GetWorld());
  effect->SetPV(provider->GetPV());
  effect->SetDirLight(ctx->GetInternalLight());
}

// class LightController
LightController::LightController(SceneRenderNode* node)
    : node_(node) {
  DCHECK(node->GetSceneNode()->type() == kLampSceneNode);
  provider_ = new LightControllerProvider(node);
}

void LightController::Update(const azer::FrameArgs& args) {
  light_mesh_->UpdateProviderParams(args);
}

void LightController::Render(azer::Renderer* renderer) {
  light_mesh_->Render(renderer);
}

// class PointLightController
PointLightController::PointLightController(SceneRenderNode* node)
    : LightController(node) {
  effect_ = CreateDiffuseEffect();
  InitMesh();
  InitControllerMesh();
}

void PointLightController::InitMesh() {
  Context* ctx = Context::instance();
  GeometryObjectPtr obj = new SphereObject(effect_->GetVertexDesc(), 0.1f);
  MeshPartPtr part = obj->CreateObject(effect_.get());
  light_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  light_mesh_->AddMeshPart(part.get());
  light_mesh_->AddProvider(provider_.get());
}

void PointLightController::InitControllerMesh() {
  SceneNode* scene_node = node_->GetSceneNode();
  DCHECK(scene_node->type() == kLampSceneNode);
  Light* light = scene_node->mutable_data()->light();
  float range = light->point_light().atten.range;
  Context* ctx = Context::instance();
  BlendingPtr blending = ctx->GetDefaultBlending();
  GeometryObjectPtr obj = new SphereObject(effect_->GetVertexDesc(), range);
  MeshPartPtr part = obj->CreateObject(effect_.get());
  part->SetBlending(blending.get());
  controller_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  controller_mesh_->AddMeshPart(part);
  controller_mesh_->AddProvider(provider_.get());
}

void PointLightController::Update(const azer::FrameArgs& args) {
  LightController::Update(args);
}

void PointLightController::Render(azer::Renderer* renderer) {
  LightController::Render(renderer);
  if (node_->GetSceneNode()->picked()) {
    controller_mesh_->Render(renderer);
  }
}

// class SpotLightController
const float SpotLightController::kTopRadius = 0.2f;
SpotLightController::SpotLightController(SceneRenderNode* node)
    : LightController(node) {
  provider_->SetLocalTransform(std::move(RotateX(Degree(90.0f))));
  effect_ = CreateDiffuseEffect();
  InitMesh();
  InitControllerMesh();
}

void SpotLightController::InitMesh() {
  Context* ctx = Context::instance();
  light_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  float kSpotHeight = 0.7f;
  float kSpotRadius = 0.3f;
  float kBaseHeight = 0.3f;
  float kBaseRadius = 0.1f;

  RenderSystem* rs = RenderSystem::Current();
  VertexDesc* desc = effect_->GetVertexDesc();
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
    EntityPtr entity(new Entity(vb, ib));
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
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    light_mesh_->AddMeshPart(part);
  }

  light_mesh_->AddProvider(provider_.get());
}

void SpotLightController::InitControllerMesh() {
  Context* ctx = Context::instance();
  controller_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  SceneNode* scene_node = node_->GetSceneNode();
  DCHECK(scene_node->type() == kLampSceneNode);
  BlendingPtr blending = ctx->GetDefaultBlending();
  Light* light = scene_node->mutable_data()->light();
  const SpotLight& spot = light->spot_light();
  float range = spot.range;
  
  float top_radius = kTopRadius;
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
  controller_mesh_->AddMeshPart(inner_cone);
  controller_mesh_->AddMeshPart(outer_cone);
  controller_mesh_->AddProvider(provider_.get());
}

void SpotLightController::Update(const azer::FrameArgs& args) {
  LightController::Update(args);
}

void SpotLightController::Render(azer::Renderer* renderer) {
  {
    ScopedCullingMode scoped_cull(kCullNone, renderer);
    LightController::Render(renderer);
  }

  if (node_->GetSceneNode()->picked()) {
    controller_mesh_->Render(renderer);
  }
}
/*
void SpotLightControllerMesh::CreateCircles(float mid, Light* light) {
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
  VertexDesc* desc = effect_->GetVertexDesc();
  GeometryObjectPtr inner_obj = new CircleObject(desc, 32, mid_inner, range - mid);
  GeometryObjectPtr outer_obj = new CircleObject(desc, 32, mid_outer, range - mid);
  MeshPartPtr inner_circle = outer_obj->CreateObject(effect_.get());
  MeshPartPtr outer_circle = outer_obj->CreateObject(effect_.get());
  picked_part_.push_back(inner_circle);
  picked_part_.push_back(outer_circle);
}
*/

// class DirLightController
DirLightController::DirLightController(SceneRenderNode* node)
    : LightController(node) {
  effect_ = CreateDiffuseEffect();
  provider_->SetLocalTransform(std::move(RotateX(Degree(90.0f))));
  InitMesh();
  InitControllerMesh();
}

void DirLightController::InitMesh() {
  const float kConeHeight = 0.3f;
  const float kConeRadius = 0.2f;
  const float kConeY = 0.8f;
  const float kCylinderRadius = 0.08f;
  Context* ctx = Context::instance();
  light_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
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
    EntityPtr entity(new Entity(vb, ib));
    *entity->mutable_vmin() = vmin;
    *entity->mutable_vmax() = vmax;
    part->AddEntity(entity);
    light_mesh_->AddMeshPart(part);
  }

  light_mesh_->AddProvider(provider_.get());
}

void DirLightController::InitControllerMesh() {
}

void DirLightController::Update(const azer::FrameArgs& args) {
  LightController::Update(args);
}

void DirLightController::Render(azer::Renderer* renderer) {
  LightController::Render(renderer);
}
}  // namespace lord
