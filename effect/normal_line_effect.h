#pragma once

#include "base/memory/ref_counted.h"
#include "azer/math/math.h"
#include "azer/render/render.h"
#include "lordaeron/effect/light.h"

namespace lord {
class NormalLineEffect : public azer::Effect {
 public:
  static const char kEffectName[];
  NormalLineEffect();
  ~NormalLineEffect();

  const char* GetEffectName() const override;
  bool Init(azer::VertexDesc* desc, const azer::Shaders& source) override;

#pragma pack(push, 4)
  struct vs_cbuffer {
    azer::Matrix4 pvw;
    azer::Matrix4 world;
    float linelength;
  };

  struct ps_cbuffer {
    azer::Vector4 color;
  };
#pragma pack(pop)

  void SetPV(const azer::Matrix4& value);
  void SetWorld(const azer::Matrix4& value);
  void SetColor(const azer::Vector4& value);
  void SetNormalLineLength(float len) { line_length_ = len;}
 protected:
  void ApplyGpuConstantTable(azer::Renderer* renderer) override;
  void InitTechnique(const azer::Shaders& source);
  void InitGpuConstantTable();

  azer::Matrix4 pv_;
  azer::Matrix4 world_;
  azer::Vector4 color_;
  float line_length_;
  DISALLOW_COPY_AND_ASSIGN(NormalLineEffect);
};

using azer::Effect;
using azer::EffectParamsProvider;
using azer::EffectAdapterKey;
using azer::EffectParamsAdapter;

class NormalLineEffectAdapter : public EffectParamsAdapter {
 public:
  NormalLineEffectAdapter();
  EffectAdapterKey key() const override;
  void Apply(Effect* e, const EffectParamsProvider* params) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(NormalLineEffectAdapter);
};

typedef scoped_refptr<NormalLineEffect> NormalLineEffectPtr;
NormalLineEffectPtr CreateNormalLineEffect();
}  // namespace lord
