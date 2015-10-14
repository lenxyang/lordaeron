#pragma once

#include "azer/render/render.h"

namespace lord {
struct EntityData {
  azer::VertexDataPtr vdata;
  azer::IndicesDataPtr idata;
  azer::EffectPtr effect;
  std::vector<azer::EffectParamsProviderPtr> provider;
  std::vector<azer::EffectParamsAdapter*> adapter;
};

struct EntityBuffer {
  azer::VertexBufferPtr vdata;
  azer::IndicesBufferPtr idata;
  azer::EffectPtr effect;
  std::vector<azer::EffectParamsProviderPtr> provider;
  std::vector<azer::EffectParamsAdapter*> adapter;
};
}  // namespace lord
