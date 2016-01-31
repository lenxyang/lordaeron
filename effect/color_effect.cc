#include "lordaeron/effect/color_effect.h"

#include <stddef.h>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/render/render.h"

using namespace azer;

namespace lord {
const char ColorEffect::kEffectName[] = "ColorEffect";
ColorEffect::ColorEffect() {}
ColorEffect::~ColorEffect() {}

const char* ColorEffect::GetEffectName() const {  return kEffectName;}
bool ColorEffect::Init(azer::VertexDesc* desc, const Shaders& sources) {
  DCHECK(sources.size() == kRenderPipelineStageNum);
  DCHECK(!sources[kVertexStage].code.empty());
  DCHECK(!sources[kPixelStage].code.empty());
  DCHECK(desc);
  vertex_desc_ = desc;
  InitTechnique(sources);
  InitGpuConstantTable();
  return true;
}

void ColorEffect::InitGpuConstantTable() {
  RenderSystem* rs = RenderSystem::Current();
  // generate GpuTable init for stage kVertexStage
  GpuConstantsTable::Desc vs_table_desc[] = {
    GpuConstantsTable::Desc("pvw", GpuConstantsType::kMatrix4,
                            offsetof(vs_cbuffer, pvw), 1),
  };
  gpu_table_[kVertexStage] = rs->CreateGpuConstantsTable(
      arraysize(vs_table_desc), vs_table_desc);
  // generate GpuTable init for stage kPixelStage
  GpuConstantsTable::Desc ps_table_desc[] = {
    GpuConstantsTable::Desc("color", GpuConstantsType::kVector4,
                            offsetof(ps_cbuffer, color), 1),
  };
  gpu_table_[kPixelStage] = rs->CreateGpuConstantsTable(
      arraysize(ps_table_desc), ps_table_desc);
}
void ColorEffect::InitTechnique(const Shaders& sources) {
  InitShaders(sources);
}

void ColorEffect::ApplyGpuConstantTable(Renderer* renderer) {
  {
    GpuConstantsTable* tb = gpu_table_[(int)kVertexStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, &pvw_, sizeof(Matrix4));
  }
  {
    GpuConstantsTable* tb = gpu_table_[(int)kPixelStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, &color_, sizeof(Vector4));
  }
}

namespace {
const VertexDesc::Desc kVertexDesc[] = {{"POSITION", 0, kVec4},};
}
ColorEffectPtr CreateColorEffect() {
  Shaders shaders;
  CHECK(LoadStageShader(kVertexStage, "lordaeron/effect/color.hlsl.vs",&shaders));
  CHECK(LoadStageShader(kPixelStage, "lordaeron/effect/color.hlsl.ps", &shaders));
  VertexDescPtr vertex_desc(new VertexDesc(kVertexDesc, 1));
  ColorEffectPtr ptr(new ColorEffect);
  ptr->Init(vertex_desc, shaders);
  return ptr;
}

}  // namespace lord
