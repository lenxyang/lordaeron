#include "lordaeron/sandbox/lighting/effect.h"

#include <stddef.h>

#include "base/basictypes.h"
#include "base/logging.h"

#include "azer/render/render.h"
#include "azer/render/util/effects/vertex_desc.h"
#include "azer/render/util/shader_util.h"

using namespace azer;

namespace lord {
namespace sandbox {
const char MyEffect::kEffectName[] = "MyEffect";
MyEffect::MyEffect(VertexDescPtr desc) 
    : Effect(RenderSystem::Current()) {
  vertex_desc_ptr_ = desc;
}

MyEffect::~MyEffect() {
}

const char* MyEffect::GetEffectName() const {
  return kEffectName;
}
bool MyEffect::Init(const ShaderPrograms& sources) {
  DCHECK(sources.size() == kRenderPipelineStageNum);
  DCHECK(!sources[kVertexStage].code.empty());
  DCHECK(!sources[kPixelStage].code.empty());
  InitTechnique(sources);
  InitGpuConstantTable();
  return true;
}

void MyEffect::InitGpuConstantTable() {
  // generate GpuTable init for stage kVertexStage
  GpuConstantsTable::Desc vs_table_desc[] = {
    GpuConstantsTable::Desc("pvw", GpuConstantsType::kMatrix4,
                            offsetof(vs_cbuffer, pvw), 1),
    GpuConstantsTable::Desc("world", GpuConstantsType::kMatrix4,
                            offsetof(vs_cbuffer, world), 1),
    GpuConstantsTable::Desc("camerapos", GpuConstantsType::kVector4,
                            offsetof(vs_cbuffer, camerapos), 1),
  };
  gpu_table_[kVertexStage] = render_system_->CreateGpuConstantsTable(
      arraysize(vs_table_desc), vs_table_desc);
  // generate GpuTable init for stage kPixelStage
  GpuConstantsTable::Desc ps_table_desc[] = {
    GpuConstantsTable::Desc("light", offsetof(ps_cbuffer, light),
                            sizeof(lord::DirLight), 1),
    GpuConstantsTable::Desc("color", GpuConstantsType::kVector4,
                            offsetof(ps_cbuffer, color), 1),
  };
  gpu_table_[kPixelStage] = render_system_->CreateGpuConstantsTable(
      arraysize(ps_table_desc), ps_table_desc);
}
void MyEffect::InitTechnique(const ShaderPrograms& sources) {
  InitShaders(sources);
}

void MyEffect::SetPV(const Matrix4& value) {
  pv_ = value;
}
void MyEffect::SetWorld(const Matrix4& value) {
  world_ = value;
}
void MyEffect::SetCameraPos(const Vector4& value) {
  camerapos_ = value;
}
void MyEffect::SetColor(const Vector4& value) {
  color_ = value;
}
void MyEffect::SetDirLight(const lord::DirLight& value) {
  light_ = value;
}

void MyEffect::ApplyGpuConstantTable(Renderer* renderer) {
  {
    Matrix4 pvw = std::move(pv_ * world_);
    GpuConstantsTable* tb = gpu_table_[(int)kVertexStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, &pvw, sizeof(Matrix4));
    tb->SetValue(1, &world_, sizeof(Matrix4));
    tb->SetValue(2, &camerapos_, sizeof(Vector4));
  }
  {
    GpuConstantsTable* tb = gpu_table_[(int)kPixelStage].get();
    DCHECK(tb != NULL);
    tb->SetValue(0, &light_, sizeof(lord::DirLight));
    tb->SetValue(1, &color_, sizeof(Vector4));
  }
}

MyEffectPtr CreateMyEffect() {
  Effect::ShaderPrograms shaders;
  CHECK(LoadShaderAtStage(kVertexStage, 
                          "lordaeron/sandbox/lighting/effect.hlsl.vs",
                          &shaders));
  CHECK(LoadShaderAtStage(kPixelStage, 
                          "lordaeron/sandbox/lighting/effect.hlsl.ps",
                          &shaders));
  MyEffectPtr ptr(new MyEffect(PosNormalVertex::CreateVertexDesc()));
  ptr->Init(shaders);
  return ptr;
}
}  // namespace sandbox
}  // namespace lord
