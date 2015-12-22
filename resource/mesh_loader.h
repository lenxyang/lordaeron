#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/resource/scene_loader.h"

namespace lord {
class MeshLoader : public ResourceSpecialLoader {
 public:
  static const char kSpecialLoaderName[];
  MeshLoader();
  ~MeshLoader();

  const char* GetLoaderName() const override;
  Resource Load(const azer::ConfigNode* node, ResourceLoaderContext* ctx) override;
  bool CouldLoad(azer::ConfigNode* node) const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(MeshLoader);
};
}  // namespace lord
