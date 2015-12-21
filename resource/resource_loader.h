#pragma once

#include "azer/base/config_node.h"
#include "azer/render/render.h"

namespace lord {
class ResourceLoader {
 public:
  ResourceLoader(azer::FileSystem* fs);
  ~ResourceLoader();

  void RegisterLoader(ResourceLoader* loader);
  azer::ResourcePtr LoadResource(const std::string& path);
 private:
  DISALLOW_COPY_AND_ASSIGN(ResourceLoader);
};
}  // namespace lord
