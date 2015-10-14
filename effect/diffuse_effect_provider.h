#pragma once

#include "azer/render/render.h"
namespace lord {
class MaterialEffectProvider : public EffectParamsProvider {
 public:
  MaterialEffectProvider(){  }
  
  void UpdateParams(const FrameArgs& args) {}
};

}  // namespace lord
