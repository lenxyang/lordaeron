#include "lordaeron/effect/diffuse_effect.h"

#include <stddef.h>

#include "base/basictypes.h"
#include "base/logging.h"

#include "azer/render/render.h"
#include "azer/render/util/effects/vertex_desc.h"
#include "lordaeron/scene/render_env_node.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/ui_scene_render.h"
#include "azer/render/util/shader_util.h"

using namespace azer;

namespace lord {
const char DiffuseEffect::kEffectName[] = "DiffuseEffect";
DiffuseEffect::DiffuseEffect() {
  emission_ = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
}

DiffuseEffect::~DiffuseEffect() {
}

const char* DiffuseEffect::GetEffectName() const {
  return kEffectName;
}
bool DiffuseEffect::Init(azer::VertexDesc* desc, const ShaderPrograms& sources) {
  DCHECK(sources.size() == kRenderPipelineStageNum);
  DCHECK(!sources[kVertexStage].code.empty());
  DCHECK(!sources[kPixelStage].code.empty());
  DCHECK(desc);
  vertex_desc_ = desc;
  InitTechnique(sources);
  InitGpuConstantTable();
  return true;
}

void DiffuseEffect::InitGpuConstantTable() {
  RenderSystem* rs = RenderSystem::Current();
  // generate GpuTable init for stage kVertexStage
  GpuConstantsTable::Desc vs_table_desc[] = {
    GpuConstantsTable::Desc("pvw", GpuConstantsType::kMatrix4,
                            offsetof(vs_cbuffer, pvw), 1),
    GpuConstantsTable::Desc("world", GpuConstantsType::kMatrix4,
                            offsetof(vs_cbuffer, world), 1),
  };
  gpu_table_[kVertexStage] = rs->CreateGpuConstantsTable(
      arraysize(vs_table_desc), vs_table_desc);
  // generate GpuTable init for stage kPixelStage
  GpuConstantsTable::Desc ps_table_desc[] = {
    GpuConstantsTable::Desc("color", GpuConstantsType::kVector4,
                            offsetof(ps_cbuffer, color), 1),
    GpuConstantsTable::Desc("emission", GpuConstantsType::kVector4,
                            offsetof(ps_cbuffer, emission), 1),
    GpuConstantsTable::Desc("light", offsetof(ps_cbuffer, light),
                            sizeof(lord::DirLight), 1),
  };
  gpu_table_[kPixelStage] = rs->CreateGpuConstantsTable(
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
void DiffuseEffect::SetEmission(const Vector4& value) {
  emission_ = value;
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
    tb->SetValue(1, &emission_, sizeof(Vector4));
    tb->SetValue(2, &light_, sizeof(lord::DirLight));
  }
}

DiffuseEffectPtr CreateDiffuseEffect() {
  Effect::ShaderPrograms shaders;
  CHECK(LoadShaderAtStage(kVertexStage, "lordaeron/effect/colored_diffuse.hlsl.vs",
                          &shaders));
  CHECK(LoadShaderAtStage(kPixelStage, "lordaeron/effect/colored_diffuse.hlsl.ps",
                          &shaders));
  DiffuseEffectPtr ptr(new DiffuseEffect);
  ptr->Init(PosNormalVertex::CreateVertexDesc(), shaders);
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
  effect->SetEmission(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
}

// class RenderNodeDiffuseEffectAdapter
RenderNodeDiffuseEffectAdapter::RenderNodeDiffuseEffectAdapter() {}
EffectAdapterKey RenderNodeDiffuseEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(RenderNode).name());
}

void RenderNodeDiffuseEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(DiffuseEffect));
  CHECK(typeid(*params) == typeid(RenderNode));
  const RenderNode* provider = (const RenderNode*)params;
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  effect->SetWorld(provider->GetWorld());
  effect->SetPV(provider->camera()->GetProjViewMatrix());
}

LordEnvNodeDiffuseEffectAdapter::LordEnvNodeDiffuseEffectAdapter() {
}

EffectAdapterKey LordEnvNodeDiffuseEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(LordEnvNodeDelegate).name());
}

void LordEnvNodeDiffuseEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(DiffuseEffect));
  CHECK(typeid(*params) == typeid(LordEnvNodeDelegate));
  const LordEnvNodeDelegate* provider = (const LordEnvNodeDelegate*)params;
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
