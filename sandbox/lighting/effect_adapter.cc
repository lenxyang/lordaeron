#include "lordaeron/sandbox/lighting/effect_adapter.h"

#include "azer/render/util.h"
#include "lordaeron/sandbox/lighting/effect.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/render_env_node.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/ui_scene_render.h"

namespace lord {
namespace sandbox {
using namespace azer;
ColorEffectAdapter::ColorEffectAdapter() {
}

EffectAdapterKey ColorEffectAdapter::key() const {
  return std::make_pair(typeid(MyEffect).name(),
                        typeid(ColorMaterial).name());
}

void ColorEffectAdapter::Apply(Effect* e, const EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(MyEffect));
  CHECK(typeid(*params) == typeid(ColorMaterial));
  ColorMaterial* provider = (ColorMaterial*)params;
  MyEffect* effect = dynamic_cast<MyEffect*>(e);
  effect->SetColor(provider->color());
}

// class RenderNodeMyEffectAdapter
RenderNodeEffectAdapter::RenderNodeEffectAdapter() {}
EffectAdapterKey RenderNodeEffectAdapter::key() const {
  return std::make_pair(typeid(MyEffect).name(),
                        typeid(RenderNode).name());
}

void RenderNodeEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(MyEffect));
  CHECK(typeid(*params) == typeid(RenderNode));
  const RenderNode* provider = (const RenderNode*)params;
  MyEffect* effect = dynamic_cast<MyEffect*>(e);
  effect->SetWorld(provider->GetWorld());
  effect->SetPV(provider->camera()->GetProjViewMatrix());
  effect->SetCameraPos(Vector4(provider->camera()->position(), 1.0f));
}

LordEnvNodeDelegateEffectAdapter::LordEnvNodeDelegateEffectAdapter() {
}

EffectAdapterKey LordEnvNodeDelegateEffectAdapter::key() const {
  return std::make_pair(typeid(MyEffect).name(),
                        typeid(LordEnvNodeDelegate).name());
}

void LordEnvNodeDelegateEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(MyEffect));
  CHECK(typeid(*params) == typeid(LordEnvNodeDelegate));
  const LordEnvNodeDelegate* provider = (const LordEnvNodeDelegate*)params;
  MyEffect* effect = dynamic_cast<MyEffect*>(e);
  for (auto iter = provider->lights().begin(); 
       iter != provider->lights().end();
       ++iter) {
    Light* light = iter->get();
    if (light->type() == kDirectionalLight) {
      effect->SetDirLight(light->dir_light());
    } else if (light->type() == kPointLight) {
      effect->SetPointLight(light->point_light());
    } else if (light->type() == kSpotLight) {
      effect->SetSpotLight(light->spot_light());
    }
  }
}
}  // namespace sandbox
}  // namespace lord
