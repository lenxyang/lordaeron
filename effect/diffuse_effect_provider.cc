#include "lordaeron/effect/diffuse_effect_provider.h"

namespace lord {
DiffuseEffectProvider::DiffuseEffectProvider() {
  color_ = azer::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
}

DiffuseEffectProvider::~DiffuseEffectProvider() {
}

void DiffuseEffectProvider::UpdateParams(const azer::FrameArgs& args) {
}
}  // namespace lord
