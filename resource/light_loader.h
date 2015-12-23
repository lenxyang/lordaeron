#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/resource/scene_loader.h"

namespace lord {
using azer::ConfigNode;
class LightLoader : public ResourceSpecialLoader {
 public:
  static const char kSpecialLoaderName[];
  LightLoader();
  ~LightLoader();

  const char* GetLoaderName() const override;
  VariantResource Load(const ConfigNode* node, ResourceLoadContext* ctx) override;
  bool CouldLoad(ConfigNode* node) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LightLoader);
};
}  // namespace lord
