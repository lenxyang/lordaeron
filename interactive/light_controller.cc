#include "lordaeron/interactive/light_controller.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "azer/render/geometry.h"
#include "lordaeron/env.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/scene_node.h"


namespace lord {
using namespace azer;
const float kControllerObjectAlpha = 0.18f;

namespace {
void MergeMeshPart(MeshPart* merge_to, MeshPart* part) {
  for (int32 i = 0; i < part->entity_count(); ++i) {
    merge_to->AddEntity(part->entity_at(i));
  }
}
}  // namespace

// LightControllerProvider
LightControllerProvider::LightControllerProvider(RenderNode* node)
    : node_(node) {
  Light* light = node->GetSceneNode()->mutable_data()->light();
  emission_ = light->diffuse() * 0.5f;
  color_ = light->diffuse();
  color_.w = kControllerObjectAlpha;
}

void LightControllerProvider::Update() {
  world_ = node_->GetWorld();
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
  GeoSphereParams param;
  param.radius = 0.1f;
  MeshPartPtr part = CreateSphereMeshPart(effect_, param, Matrix4::kIdentity);
  light_mesh_->AddMeshPart(part.get());
}

void PointLightController::InitControllerMesh() {
  SceneNode* scene_node = node_->GetSceneNode();
  DCHECK(scene_node->type() == kLampSceneNode);
  Light* light = scene_node->mutable_data()->light();
  float range = light->point_light().atten.range;
  LordEnv* ctx = LordEnv::instance();
  BlendingPtr blending = ctx->GetDefaultBlending();

  GeoSphereParams param;
  param.radius = range;
  MeshPartPtr part = CreateSphereMeshPart(effect_, param, Matrix4::kIdentity);
  light_mesh_->AddMeshPart(part.get());
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
  effect_ = CreateDiffuseEffect();
  light_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  controller_mesh_ = new Mesh(ctx->GetEffectAdapterContext());
  line_mesh_ = new Mesh(ctx->GetEffectAdapterContext());

  SceneNode* scene_node = node_->GetSceneNode();
  Light* light = scene_node->mutable_data()->light();
  InitMesh();
  InitControllerMesh();
  CreateFrameLine(light->spot_light().range, light);

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
  MeshPartPtr part(new MeshPart(effect_));
  Matrix4 rot = std::move(RotateX(Degree(-90.0f)));
  {
    // create VertexData
    GeoBarrelParams params;
    params.height = kSpotHeight;
    params.top_radius = kSpotRadius;
    params.bottom_radius = kBaseRadius;
    Matrix4 mat = rot * std::move(Translate(0.0f, -(kSpotHeight - 0.5f), 0.0f));
    MeshPartPtr ptr = CreateCylinderMeshPart(effect_, params, mat);
    MergeMeshPart(part, ptr);
  }

  {
    // create VertexData
    Matrix4 mat = rot * std::move(Translate(0.0f, -0.5f, 0.0f));
    GeoBarrelParams params;
    params.height = kSpotHeight;
    params.top_radius = kBaseRadius;
    params.bottom_radius = kBaseRadius;
    MeshPartPtr ptr = CreateCylinderMeshPart(effect_, params, mat);
    MergeMeshPart(part, ptr);
  }
  light_mesh_->AddMeshPart(part);
}

void SpotLightController::InitControllerMesh() {
  LordEnv* ctx = LordEnv::instance();
  SceneNode* scene_node = node_->GetSceneNode();
  Light* light = scene_node->mutable_data()->light();
  DCHECK(scene_node->type() == kLampSceneNode);
  BlendingPtr blending = ctx->GetDefaultBlending();
  const SpotLight& spot = light->spot_light();
  float range = spot.range;
  Matrix4 mat = std::move(RotateX(Degree(-90.0f)));
  float top_radius = kTopRadius;
  float inner_sine = std::sqrt(1 - spot.theta * spot.theta);
  float inner_radius = range * inner_sine / spot.theta;
  float outer_sine = std::sqrt(1 - spot.phi * spot.phi);
  float outer_radius = range * outer_sine / spot.phi;

  MeshPartPtr part(new MeshPart(effect_));
  VertexDesc* desc = effect_->vertex_desc();
  GeoBarrelParams params;
  params.height = range;
  params.top_radius = inner_radius;
  params.bottom_radius = top_radius;
  EntityPtr ptr1 = CreateBarrelEntity(desc, params, mat);
  params.top_radius = outer_radius;
  params.bottom_radius = top_radius;
  EntityPtr ptr2 = CreateBarrelEntity(desc, params, mat);
  part->AddEntity(ptr1);
  part->AddEntity(ptr2);
  part->SetBlending(blending.get());
  controller_mesh_->AddMeshPart(part);
}

void SpotLightController::Update(const FrameArgs& args) {
  LightController::Update(args);
}

void SpotLightController::Render(Renderer* renderer) {
  {
    ScopedRasterizerState scoped_cull(renderer, rasterizer_state_);
    LightController::Render(renderer);
    if (node_->GetSceneNode()->picked()) {
      line_mesh_->Render(renderer);
      controller_mesh_->Render(renderer);
    }
  }
}

void SpotLightController::CreateFrameLine(float mid, Light* light) {
  RenderSystem* rs = RenderSystem::Current();
  const SpotLight& spot = light->spot_light();
  Matrix4 mat = std::move(RotateX(Degree(-90.0f)));
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
  Matrix4 circle_mat = mat * Translate(0.0f, mid, 0.0f);
  EntityPtr entity1 = CreateCircleEntity(desc, mid_inner, kSlice, circle_mat);
  EntityPtr entity2 = CreateCircleEntity(desc, mid_outer, kSlice, circle_mat);
  Vector3 points[] = {
    Vector3(-mid_outer, mid, 0.0f),
    Vector3( mid_outer, mid, 0.0f),
    Vector3(0.0f, mid, -mid_outer),
    Vector3(0.0f, mid,  mid_outer),
    Vector3(-top_radius, 0.0f, 0.0f),
    Vector3(-outer_radius, spot.range, 0.0f),
    Vector3( top_radius, 0.0f, 0.0f),
    Vector3( outer_radius, spot.range, 0.0f),
    Vector3(0.0f, 0.0f,       -top_radius),
    Vector3(0.0f, spot.range, -outer_radius),
    Vector3(0.0f, 0.0f,        top_radius),
    Vector3(0.0f, spot.range,  outer_radius),
  };
  EntityPtr cross_entity = CreateGeoPointsList(points, arraysize(points), desc, 
                                               mat);
  cross_entity->set_topology(kLineList);
  MeshPartPtr part(new MeshPart(effect_));
  part->AddEntity(cross_entity);
  part->AddEntity(entity1);
  part->AddEntity(entity2);
  line_mesh_->AddMeshPart(part);
}

// class DirLightController
DirLightController::DirLightController(RenderNode* node)
    : LightController(node) {
  LordEnv* ctx = LordEnv::instance();
  effect_ = CreateDiffuseEffect();
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
  GeoAxisParams params;
  params.axis_length = 0.8f;
  params.axis_radius = 0.08f;
  params.cone_radius = 0.2f;
  params.cone_height = 0.3f;
  params.slice = 24;
  Matrix4 mat = std::move(RotateX(Degree(-90.0f)));
  MeshPartPtr part = CreateAxisMeshPart(effect_, params, mat);
  light_mesh_->AddMeshPart(part);
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
