#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"

namespace lord {
namespace sandbox {

using azer::Effect;
using azer::EffectParamsProvider;
using azer::EffectAdapterKey;
using azer::EffectParamsAdapter;

class ColorEffectAdapter : public EffectParamsAdapter {
 public:
  ColorEffectAdapter();

  EffectAdapterKey key() const override;
  void Apply(Effect* e, const EffectParamsProvider* params) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(ColorEffectAdapter);
};

class RenderNodeEffectAdapter : public EffectParamsAdapter {
 public:
  RenderNodeEffectAdapter();

  EffectAdapterKey key() const override;
  void Apply(Effect* e, const EffectParamsProvider* params) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(RenderNodeEffectAdapter);
};

class LordEnvNodeDelegateEffectAdapter : public EffectParamsAdapter {
 public:
  LordEnvNodeDelegateEffectAdapter();

  EffectAdapterKey key() const override;
  void Apply(Effect* e, const EffectParamsProvider* params) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LordEnvNodeDelegateEffectAdapter);
};
}  // namespace sandbox
}  // namespace lord
