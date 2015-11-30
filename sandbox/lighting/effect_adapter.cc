#include "lordaeron/sandbox/lighting/effect_adapter.h"

#include "azer/render/util.h"
#include "lordaeron/sandbox/lighting/effect.h"
#include "lordaeron/effect/diffuse_effect_provider.h"
#include "lordaeron/effect/global_environemnt_params.h"
#include "lordaeron/effect/scene_node_params.h"

namespace lord {
namespace sandbox {
ColorEffectAdapter::ColorEffectAdapter() {
}

azer::EffectAdapterKey ColorEffectAdapter::key() const {
  return std::make_pair(typeid(MyEffect).name(),
                        typeid(DiffuseEffectProvider).name());
}

void ColorEffectAdapter::Apply(azer::Effect* e, 
                               const azer::EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(MyEffect));
  CHECK(typeid(*params) == typeid(DiffuseEffectProvider));
  DiffuseEffectProvider* provider = (DiffuseEffectProvider*)params;
  MyEffect* effect = dynamic_cast<MyEffect*>(e);
  effect->SetColor(provider->color());
}

// class SceneNodeColorEffectAdapter
SceneNodeColorEffectAdapter::SceneNodeColorEffectAdapter() {
}

azer::EffectAdapterKey SceneNodeColorEffectAdapter::key() const {
  return std::make_pair(typeid(MyEffect).name(),
                        typeid(SceneNodeParams).name());
}

void SceneNodeColorEffectAdapter::Apply(
    azer::Effect* e, const azer::EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(MyEffect));
  CHECK(typeid(*params) == typeid(SceneNodeParams));
  SceneNodeParams* provider = (SceneNodeParams*)params;
  MyEffect* effect = dynamic_cast<MyEffect*>(e);

  effect->SetWorld(provider->world());
  // effect->SetDirLight(*(provider->light()));
}

// class GlobalEnvColorEffectAdapter
GlobalEnvColorEffectAdapter::GlobalEnvColorEffectAdapter() {
}

azer::EffectAdapterKey GlobalEnvColorEffectAdapter::key() const {
  return std::make_pair(typeid(MyEffect).name(),
                        typeid(GlobalEnvironmentParams).name());
}

void GlobalEnvColorEffectAdapter::Apply(
    azer::Effect* e, const azer::EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(MyEffect));
  CHECK(typeid(*params) == typeid(GlobalEnvironmentParams));
  GlobalEnvironmentParams* provider = (GlobalEnvironmentParams*)params;
  MyEffect* effect = dynamic_cast<MyEffect*>(e);

  effect->SetCameraPos(azer::Vector4(provider->camera()->position(), 1.0f));
  effect->SetPV(provider->GetProjView());
  effect->SetDirLight(provider->light()->dir_light());
}
}  // namespace sandbox
}  // namespace lord
