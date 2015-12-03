#include "lordaeron/sandbox/lighting/effect_adapter.h"

#include "azer/render/util.h"
#include "lordaeron/sandbox/lighting/effect.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
namespace sandbox {
using namespace azer;
ColorEffectAdapter::ColorEffectAdapter() {
}

EffectAdapterKey ColorEffectAdapter::key() const {
  return std::make_pair(typeid(MyEffect).name(),
                        typeid(DiffuseColorProvider).name());
}

void ColorEffectAdapter::Apply(Effect* e, const EffectParamsProvider* params) const  {
  CHECK(typeid(*e) == typeid(MyEffect));
  CHECK(typeid(*params) == typeid(DiffuseColorProvider));
  DiffuseColorProvider* provider = (DiffuseColorProvider*)params;
  MyEffect* effect = dynamic_cast<MyEffect*>(e);
  effect->SetColor(provider->color());
}

}  // namespace sandbox
}  // namespace lord
