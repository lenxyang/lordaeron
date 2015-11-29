#include "lordaeron/lighting/effects/diffuse_effect_provider.h"

namespace lord {
namespace sandbox {
DiffuseEffectProvider::DiffuseEffectProvider() {
  color_ = azer::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
}

DiffuseEffectProvider::~DiffuseEffectProvider() {
}

void DiffuseEffectProvider::UpdateParams(const azer::FrameArgs& args) {
}
}  // namespace sandbox
}  // namespace lord
