#include "lordaeron/effect/adapter/diffuse_effect_adapter.h"

#include "azer/render/util.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/effect/diffuse_effect_provider.h"
#include "lordaeron/effect/scene_node_params.h"

namespace lord {
ColorEffectAdapter::ColorEffectAdapter() {
}

azer::EffectAdapterKey ColorEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(DiffuseEffectProvider).name());
}

void ColorEffectAdapter::Apply(azer::Effect* e, 
                               const azer::EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(DiffuseEffect));
  CHECK(typeid(*params) == typeid(DiffuseEffectProvider));
  DiffuseEffectProvider* provider = (DiffuseEffectProvider*)params;
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  effect->SetColor(provider->color());
}

// class SceneNodeColorEffectAdapter
SceneNodeColorEffectAdapter::SceneNodeColorEffectAdapter() {
}

azer::EffectAdapterKey SceneNodeColorEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(SceneNodeParams).name());
}

void SceneNodeColorEffectAdapter::Apply(
    azer::Effect* e, const azer::EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(DiffuseEffect));
  CHECK(typeid(*params) == typeid(SceneNodeParams));
  SceneNodeParams* provider = (SceneNodeParams*)params;
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);

  effect->SetWorld(provider->world());
  // effect->SetDirLight(*(provider->light()));
}

}  // namespace lord
