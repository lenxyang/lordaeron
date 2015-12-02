#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"

namespace lord {
class ColorEffectAdapter : public azer::EffectParamsAdapter {
 public:
  ColorEffectAdapter();

  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* e, const azer::EffectParamsProvider* params) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(ColorEffectAdapter);
};

class SceneNodeColorEffectAdapter : public azer::EffectParamsAdapter {
 public:
  SceneNodeColorEffectAdapter();

  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* e, const azer::EffectParamsProvider* params) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(SceneNodeColorEffectAdapter);
};

}  // namespace lord
