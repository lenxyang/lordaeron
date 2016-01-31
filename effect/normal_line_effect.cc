#include "lordaeron/effect/normal_line_effect.h"

#include <stddef.h>

#include "base/basictypes.h"
#include "base/logging.h"

#include "azer/render/render.h"
#include "azer/render/util/effects/vertex_desc.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/scene_node.h"

using namespace azer;

namespace lord {
const char NormalLineEffect::kEffectName[] = "NormalLineEffect";
NormalLineEffect::NormalLineEffect() : line_length_(1.0f) {}

NormalLineEffect::~NormalLineEffect() {}

const char* NormalLineEffect::GetEffectName() const {
  return kEffectName;
}
bool NormalLineEffect::Init(VertexDesc* desc, const Shaders& sources) {
  DCHECK(desc);
  DCHECK(sources.size() == kRenderPipelineStageNum);
  DCHECK(!sources[kVertexStage].code.empty());
  DCHECK(!sources[kGeometryStage].code.empty());
  DCHECK(!sources[kPixelStage].code.empty());
  vertex_desc_ = desc;
  InitTechnique(sources);
  InitGpuConstantTable();
  return true;
}

void NormalLineEffect::InitGpuConstantTable() {
  RenderSystem* rs = RenderSystem::Current();
  // generate GpuTable init for stage kVertexStage
  GpuConstantsTable::Desc vs_table_desc[] = {
    GpuConstantsTable::Desc("pvw", GpuConstantsType::kMatrix4,
                            offsetof(vs_cbuffer, pvw), 1),
    GpuConstantsTable::Desc("world", GpuConstantsType::kMatrix4,
                            offsetof(vs_cbuffer, world), 1),
    GpuConstantsTable::Desc("linelength", GpuConstantsType::kFloat,
                            offsetof(vs_cbuffer, linelength), 1),
  };
  gpu_table_[kVertexStage] = rs->CreateGpuConstantsTable(
      arraysize(vs_table_desc), vs_table_desc);


  // generate GpuTable init for stage kPixelStage
  GpuConstantsTable::Desc ps_table_desc[] = {
    GpuConstantsTable::Desc("color", GpuConstantsType::kVector4, 0, 1),
  };
  gpu_table_[kPixelStage] = rs->CreateGpuConstantsTable(
      arraysize(ps_table_desc), ps_table_desc);
}
void NormalLineEffect::InitTechnique(const Shaders& sources) {
  InitShaders(sources);
}

void NormalLineEffect::SetPV(const Matrix4& value) {
  pv_ = value;
}

void NormalLineEffect::SetWorld(const Matrix4& value) {
  world_ = value;
}

void NormalLineEffect::SetColor(const Vector4& value) {
  color_ = value;
}

void NormalLineEffect::ApplyGpuConstantTable(Renderer* renderer) {
  Matrix4 pvw = std::move(pv_ * world_);
  {
    GpuConstantsTable* tb = gpu_table_[(int)kVertexStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, &pvw, sizeof(Matrix4));
    tb->SetValue(1, &world_, sizeof(Matrix4));
    tb->SetValue(2, &line_length_, sizeof(float));
  }

  {
    GpuConstantsTable* tb = gpu_table_[(int)kPixelStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, &color_, sizeof(Vector4));
  }
}

NormalLineEffectPtr CreateNormalLineEffect() {
  Shaders shaders;
  CHECK(LoadStageShader(kVertexStage, "lordaeron/effect/normal_line.hlsl.vs",
                        &shaders));
  CHECK(LoadStageShader(kGeometryStage, "lordaeron/effect/normal_line.hlsl.gs",
                        &shaders));
  CHECK(LoadStageShader(kPixelStage, "lordaeron/effect/normal_line.hlsl.ps",
                        &shaders));
  NormalLineEffectPtr ptr(new NormalLineEffect);
  ptr->Init(PosNormalVertex::CreateVertexDesc(), shaders);
  return ptr;
}


NormalLineEffectAdapter::NormalLineEffectAdapter() {
}

EffectAdapterKey NormalLineEffectAdapter::key() const {
  return std::make_pair(typeid(NormalLineEffect).name(),
                        typeid(RenderNode).name());
}

void NormalLineEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  CHECK(typeid(*e) == typeid(NormalLineEffect));
  CHECK(typeid(*params) == typeid(RenderNode));
  const RenderNode* provider = (const RenderNode*)params;
  NormalLineEffect* effect = dynamic_cast<NormalLineEffect*>(e);
  effect->SetWorld(provider->GetWorld());
  effect->SetPV(provider->camera()->GetProjViewMatrix());
  effect->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  effect->SetNormalLineLength(0.3f);
}
}  // namespace lord
