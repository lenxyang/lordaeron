#pragma once

#include "azer/base/config_node.h"
#include "azer/render/render.h"

namespace lord {
struct ResourceLoadContext;
class Material : public azer::EffectParamsProvider {
 public:
  Material() {}
  ~Material() {}

  virtual bool Init(const azer::ConfigNode* node, ResourceLoadContext* ctx) = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(Material);
};

typedef scoped_refptr<Material> MaterialPtr;
}  // namespace lord
