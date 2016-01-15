#pragma once

#include "base/memory/ref_counted.h"
#include "azer/math/math.h"
#include "azer/render/render.h"
#include "lordaeron/effect/light.h"

namespace lord {
class ColorEffect : public azer::Effect {
 public:
  static const char kEffectName[];
  ColorEffect();
  ~ColorEffect();

  const char* GetEffectName() const override;
  bool Init(azer::VertexDesc* desc, const ShaderPrograms& source) override;

#pragma pack(push, 4)
  struct vs_cbuffer {
    azer::Matrix4 pvw;
  };

  struct ps_cbuffer {
    azer::Vector4 color;
  };
#pragma pack(pop)

  void SetPVW(const azer::Matrix4& value) { pvw_ = value;}
  void SetColor(const azer::Vector4& value) { color_ = value;}
 protected:
  void ApplyGpuConstantTable(azer::Renderer* renderer) override;
  void InitTechnique(const ShaderPrograms& source);
  void InitGpuConstantTable();

  azer::Matrix4 pvw_;
  azer::Vector4 color_;
  DISALLOW_COPY_AND_ASSIGN(ColorEffect);
};

typedef scoped_refptr<ColorEffect> ColorEffectPtr;
ColorEffectPtr CreateColorEffect();
}  // namespace lord
