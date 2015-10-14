#include "lordaeron/effect/adapter/diffuse_effect_adapter.h"

#include "azer/render/util.h"
#include "lordaeron/effect/global_environemnt_params.h"
#include "lordaeron/effect/scene_node_params.h"

namespace lord {

using azer::ColoredDiffuseEffect;
ColorEffectAdapter::ColorEffectAdapter() {
}

azer::EffectAdapterKey ColorEffectAdapter::key() const {
  return std::make_pair(typeid(ColoredDiffuseEffect), typeid(ColorEffectProvider));
}

void ColorEffectAdapter::Apply(azer::Effect* e, 
                               azer::EffectParamsProvider* params) const  {
  using azer::ColoredDiffuseEffect;
  CHECK(typeid(*e) == typeid(ColoredDiffuseEffect));
  CHECK(typeid(*params) == typeid(ColorEffectProvider));
  ColorEffectProvider* provider = (ColorEffectProvider*)params;
  ColoredDiffuseEffect* effect = dynamic_cast<ColoredDiffuseEffect*>(e);
  effect->SetColor(Vector4(1.0, 0.0, 0.0, 1.0));
}

// class SceneNodeColorEffectAdapter
SceneNodeColorEffectAdapter::SceneNodeColorEffectAdapter() {
}

azer::EffectAdapterKey SceneNodeColorEffectAdapter::key() const {
  return std::make_pair(typeid(ColoredDiffuseEffect),
                        typeid(SceneNodeEffectProvider));
}

void SceneNodeColorEffectAdapter::Apply(azer::Effect* e, 
                                        azer::EffectParamsProvider* params) const  {
  using azer::ColoredDiffuseEffect;
  CHECK(typeid(*e) == typeid(ColoredDiffuseEffect));
  CHECK(typeid(*params) == typeid(SceneNodeEffectProvider));
  SceneNodeEffectProvider* provider = (SceneNodeEffectProvider*)params;
  ColoredDiffuseEffect* effect = dynamic_cast<ColoredDiffuseEffect*>(e);

  effect->SetWorld(provider->world());
  effect->SetPVW(provider->pvw());
  effect->SetDirLight(*(provider->light()));
}

// class GlobalEnvColorEffectAdapter
GlobalEnvColorEffectAdapter::GlobalEnvColorEffectAdapter() {
}

azer::EffectAdapterKey GlobalEnvColorEffectAdapter::key() const {
  return std::make_pair(typeid(ColoredDiffuseEffect),
                        typeid(GlobalEnvEffectProvider));
}

void GlobalEnvColorEffectAdapter::GlobalEnvColorEffectAdapter::Apply(
    azer::Effect* e, azer::EffectParamsProvider* params) const  {
  using azer::ColoredDiffuseEffect;
  CHECK(typeid(*e) == typeid(ColoredDiffuseEffect));
  CHECK(typeid(*params) == typeid(GlobalEnvEffectProvider));
  GlobalEnvEffectProvider* provider = (GlobalEnvEffectProvider*)params;
  ColoredDiffuseEffect* effect = dynamic_cast<ColoredDiffuseEffect*>(e);

  effect->SetWorld(provider->world());
  effect->SetPVW(provider->pvw());
  effect->SetDirLight(*(provider->light()));
}

}  // namespace lord
