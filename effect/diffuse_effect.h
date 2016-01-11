#pragma once

#include "base/memory/ref_counted.h"
#include "azer/math/math.h"
#include "azer/render/render.h"
#include "lordaeron/effect/light.h"

namespace lord {
class DiffuseEffect : public azer::Effect {
 public:
  static const char kEffectName[];
  DiffuseEffect();
  ~DiffuseEffect();

  const char* GetEffectName() const override;
  bool Init(azer::VertexDesc* desc, const ShaderPrograms& source) override;

#pragma pack(push, 4)
  struct vs_cbuffer {
    azer::Matrix4 pvw;
    azer::Matrix4 world;
  };

  struct ps_cbuffer {
    azer::Vector4 color;
    azer::Vector4 emission;
    DirLight light;
  };
#pragma pack(pop)

  void SetPV(const azer::Matrix4& value);
  void SetWorld(const azer::Matrix4& value);
  void SetColor(const azer::Vector4& value);
  void SetEmission(const azer::Vector4& value);
  void SetDirLight(const DirLight& value);
 protected:
  void ApplyGpuConstantTable(azer::Renderer* renderer) override;
  void InitTechnique(const ShaderPrograms& source);
  void InitGpuConstantTable();

  azer::Matrix4 pv_;
  azer::Matrix4 world_;
  azer::Vector4 color_;
  azer::Vector4 emission_;
  DirLight light_;
  DISALLOW_COPY_AND_ASSIGN(DiffuseEffect);
};

class DiffuseColorProvider : public azer::EffectParamsProvider {
 public:
  DiffuseColorProvider();
  ~DiffuseColorProvider();

  azer::Vector4 color() const { return color_;}
  void SetColor(const azer::Vector4& color) { color_ = color;}
 private:
  azer::Vector4 color_;
  DISALLOW_COPY_AND_ASSIGN(DiffuseColorProvider);
};

using azer::Effect;
using azer::EffectParamsProvider;
using azer::EffectAdapterKey;
using azer::EffectParamsAdapter;

class DiffuseColorEffectAdapter : public EffectParamsAdapter {
 public:
  DiffuseColorEffectAdapter();

  EffectAdapterKey key() const override;
  void Apply(Effect* e, const EffectParamsProvider* params) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(DiffuseColorEffectAdapter);
};

class RenderNodeDiffuseEffectAdapter : public EffectParamsAdapter {
 public:
  RenderNodeDiffuseEffectAdapter();

  EffectAdapterKey key() const override;
  void Apply(Effect* e, const EffectParamsProvider* params) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(RenderNodeDiffuseEffectAdapter);
};

class LordEnvNodeDiffuseEffectAdapter : public EffectParamsAdapter {
 public:
  LordEnvNodeDiffuseEffectAdapter();

  EffectAdapterKey key() const override;
  void Apply(Effect* e, const EffectParamsProvider* params) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LordEnvNodeDiffuseEffectAdapter);
};


typedef scoped_refptr<DiffuseEffect> DiffuseEffectPtr;
DiffuseEffectPtr CreateDiffuseEffect();
}  // namespace lord
