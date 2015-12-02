#include "lordaeron/effect/diffuse_effect_adapter.h"

#include "azer/render/util.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/effect/diffuse_effect_provider.h"

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
}  // namespace lord
