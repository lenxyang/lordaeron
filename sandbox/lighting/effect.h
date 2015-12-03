#pragma once

#include "base/memory/ref_counted.h"
#include "azer/math/math.h"
#include "azer/render/render.h"
#include "lordaeron/effect/light.h"

namespace lord {
namespace sandbox {
class MyEffect : public azer::Effect {
 public:
  static const char kEffectName[];
  MyEffect(azer::VertexDescPtr desc);
  ~MyEffect();

  const char* GetEffectName() const override;
  bool Init(const ShaderPrograms& source) override;

#pragma pack(push, 4)
  struct vs_cbuffer {
    azer::Matrix4 pvw;
    azer::Matrix4 world;
    azer::Vector4 camerapos;
  };

  struct ps_cbuffer {
    DirLight light;
    azer::Vector4 color;
  };
#pragma pack(pop)

  void SetPV(const azer::Matrix4& value);
  void SetWorld(const azer::Matrix4& value);
  void SetCameraPos(const azer::Vector4& CameraPos);
  void SetColor(const azer::Vector4& value);
  void SetDirLight(const DirLight& value);
 protected:
  void ApplyGpuConstantTable(azer::Renderer* renderer) override;
  void InitTechnique(const ShaderPrograms& source);
  void InitGpuConstantTable();

  azer::Matrix4 pv_;
  azer::Matrix4 world_;
  azer::Vector4 color_;
  azer::Vector4 camerapos_;
  DirLight light_;
  DISALLOW_COPY_AND_ASSIGN(MyEffect);
};

typedef scoped_refptr<MyEffect> MyEffectPtr;
MyEffectPtr CreateMyEffect();
}  // namespace sandbox
}  // namespace lord
