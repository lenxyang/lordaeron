#include "lordaeron/sandbox/lighting/effect_adapter.h"

#include "azer/render/util.h"
#include "lordaeron/sandbox/lighting/effect.h"
#include "lordaeron/effect/diffuse_effect_provider.h"

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

}  // namespace sandbox
}  // namespace lord
