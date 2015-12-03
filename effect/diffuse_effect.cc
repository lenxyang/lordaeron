#include "lordaeron/effect/diffuse_effect.h"

#include <stddef.h>

#include "base/basictypes.h"
#include "base/logging.h"

#include "azer/render/render.h"
#include "azer/render/util/effects/vertex_desc.h"
#include "azer/render/util/shader_util.h"
#include "lordaeron/scene/scene_render_tree.h"

using namespace azer;

namespace lord {
const char DiffuseEffect::kEffectName[] = "DiffuseEffect";
DiffuseEffect::DiffuseEffect(VertexDescPtr desc) 
    : Effect(RenderSystem::Current()) {
  vertex_desc_ptr_ = desc;
}

DiffuseEffect::~DiffuseEffect() {
}

const char* DiffuseEffect::GetEffectName() const {
   return kEffectName;
}
bool DiffuseEffect::Init(const ShaderPrograms& sources) {
  DCHECK(sources.size() == kRenderPipelineStageNum);
  DCHECK(!sources[kVertexStage].code.empty());
  DCHECK(!sources[kPixelStage].code.empty());
  InitTechnique(sources);
  InitGpuConstantTable();
  return true;
}

void DiffuseEffect::InitGpuConstantTable() {
  // generate GpuTable init for stage kVertexStage
  GpuConstantsTable::Desc vs_table_desc[] = {
    GpuConstantsTable::Desc("pvw", GpuConstantsType::kMatrix4,
         offsetof(vs_cbuffer, pvw), 1),
    GpuConstantsTable::Desc("world", GpuConstantsType::kMatrix4,
         offsetof(vs_cbuffer, world), 1),
  };
  gpu_table_[kVertexStage] = render_system_->CreateGpuConstantsTable(
      arraysize(vs_table_desc), vs_table_desc);
  // generate GpuTable init for stage kPixelStage
  GpuConstantsTable::Desc ps_table_desc[] = {
    GpuConstantsTable::Desc("color", GpuConstantsType::kVector4,
         offsetof(ps_cbuffer, color), 1),
    GpuConstantsTable::Desc("light", offsetof(ps_cbuffer, light),
         sizeof(lord::DirLight), 1),
  };
  gpu_table_[kPixelStage] = render_system_->CreateGpuConstantsTable(
      arraysize(ps_table_desc), ps_table_desc);
}
void DiffuseEffect::InitTechnique(const ShaderPrograms& sources) {
  InitShaders(sources);
}

void DiffuseEffect::SetPV(const Matrix4& value) {
  pv_ = value;
}
void DiffuseEffect::SetWorld(const Matrix4& value) {
  world_ = value;
}
void DiffuseEffect::SetColor(const Vector4& value) {
  color_ = value;
}
void DiffuseEffect::SetDirLight(const lord::DirLight& value) {
  light_ = value;
}

void DiffuseEffect::ApplyGpuConstantTable(Renderer* renderer) {
  {
    Matrix4 pvw = std::move(pv_ * world_);
    GpuConstantsTable* tb = gpu_table_[(int)kVertexStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, &pvw, sizeof(Matrix4));
    tb->SetValue(1, &world_, sizeof(Matrix4));
  }
  {
    GpuConstantsTable* tb = gpu_table_[(int)kPixelStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, &color_, sizeof(Vector4));
    tb->SetValue(1, &light_, sizeof(lord::DirLight));
  }
}

DiffuseEffectPtr CreateDiffuseEffect() {
  Effect::ShaderPrograms shaders;
  CHECK(LoadShaderAtStage(kVertexStage, 
                          "lordaeron/effect/hlsl/colored_diffuse.hlsl.vs",
                          &shaders));
  CHECK(LoadShaderAtStage(kPixelStage, 
                          "lordaeron/effect/hlsl/colored_diffuse.hlsl.ps",
                          &shaders));
  DiffuseEffectPtr ptr(new DiffuseEffect(
             PosNormalVertex::CreateVertexDesc()));
  ptr->Init(shaders);
  return ptr;
}

// class DiffuseColorProvider
DiffuseColorProvider::DiffuseColorProvider() {
  color_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
}
DiffuseColorProvider::~DiffuseColorProvider() {}
void DiffuseColorProvider::UpdateParams(const FrameArgs& args) {}

// class DiffuseColorEffectAdapter
DiffuseColorEffectAdapter::DiffuseColorEffectAdapter() {}

EffectAdapterKey DiffuseColorEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(DiffuseColorProvider).name());
}

void DiffuseColorEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(DiffuseEffect));
  CHECK(typeid(*params) == typeid(DiffuseColorProvider));
  DiffuseColorProvider* provider = (DiffuseColorProvider*)params;
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  effect->SetColor(provider->color());
}

// class SceneRenderNodeDiffuseEffectAdapter
SceneRenderNodeDiffuseEffectAdapter::SceneRenderNodeDiffuseEffectAdapter() {}
EffectAdapterKey SceneRenderNodeDiffuseEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(SceneRenderNode).name());
}

void SceneRenderNodeDiffuseEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(DiffuseEffect));
  CHECK(typeid(*params) == typeid(SceneRenderNode));
  const SceneRenderNode* provider = (const SceneRenderNode*)params;
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  effect->SetWorld(provider->GetWorld());
  effect->SetPV(provider->camera()->GetProjViewMatrix());
}

SceneRenderEnvNodeDiffuseEffectAdapter::SceneRenderEnvNodeDiffuseEffectAdapter() {
}

EffectAdapterKey SceneRenderEnvNodeDiffuseEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(SceneRenderEnvNode).name());
}

void SceneRenderEnvNodeDiffuseEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(DiffuseEffect));
  CHECK(typeid(*params) == typeid(SceneRenderEnvNode));
  const SceneRenderEnvNode* provider = (const SceneRenderEnvNode*)params;
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  for (auto iter = provider->lights().begin(); 
       iter != provider->lights().end();
       ++iter) {
    if ((*iter)->type() == kDirectionalLight) {
      effect->SetDirLight((*iter)->dir_light());
      break;
    }
  }
}
}  // namespace lord
