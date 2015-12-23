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
  VariantResource Load(const azer::ConfigNode* node,
                       ResourceLoadContext* ctx) override;
  bool CouldLoad(azer::ConfigNode* node) const override;
 private:
  azer::MeshPtr LoadMeshData(const azer::ConfigNode* node,
                             azer::VertexDesc* desc,
                             ResourceLoadContext* ctx);
  DISALLOW_COPY_AND_ASSIGN(MeshLoader);
};
}  // namespace lord
