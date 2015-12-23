#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/resource/scene_loader.h"

namespace lord {
class MaterialLoader : public ResourceSpecialLoader {
 public:
  static const char kSpecialLoaderName[];
  MaterialLoader();
  ~MaterialLoader();

  const char* GetLoaderName() const override;
  VariantResource Load(const azer::ConfigNode* node,
                       ResourceLoadContext* ctx) override;
  bool CouldLoad(azer::ConfigNode* node) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(MaterialLoader);
};
}  // namespace lord
