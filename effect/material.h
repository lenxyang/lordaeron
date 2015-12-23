#pragma once

#include "azer/base/config_node.h"
#include "azer/render/render.h"

namespace lord {
class Material : public azer::EffectParamsProvider {
 public:
  Material() {}
  ~Material() {}

  virtual bool Init(const azer::ConfigNode* node) = 0;
  void UpdateParams(const azer::FrameArgs& args) override {}
 private:
  DISALLOW_COPY_AND_ASSIGN(Material);
};

typedef scoped_refptr<Material> MaterialPtr;
}  // namespace lord
