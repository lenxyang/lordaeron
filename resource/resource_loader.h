#pragma once

#include <map>
#include <string>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "azer/base/repository_node.h"
#include "azer/base/config_node.h"
#include "azer/base/file_system.h"
#include "lordaeron/resource/variant_resource.h"

namespace lord {
class ResourceLoader;
class ResourceSpecialLoader;
typedef scoped_refptr<ResourceSpecialLoader> ResourceSpecialLoaderPtr;

struct ResourceLoadContext {
  ResourceLoader* loader;
  azer::ResPath path;
  azer::RepositoryNodePtr root;
  azer::FileSystem* filesystem;
};

class ResourceSpecialLoader : public ::base::RefCounted<ResourceSpecialLoader> {
 public:
  virtual const char* GetLoaderName() const = 0;
  virtual bool CouldLoad(azer::ConfigNode* node) const = 0;
  virtual VariantResource Load(const azer::ConfigNode* node,
                               ResourceLoadContext* ctx) = 0;
};

class ResourceLoader {
 public:
  explicit ResourceLoader(azer::FileSystem* fs);
  ~ResourceLoader();

  void RegisterSpecialLoader(ResourceSpecialLoader* loader);
  ResourceSpecialLoader* GetLoader(azer::ConfigNode *node);

  azer::FileSystem* file_system() { return filesystem_;}
  VariantResource Load(const azer::ResPath& path);
 private:
  std::map<std::string, ResourceSpecialLoaderPtr> dict_;
  azer::FileSystem* filesystem_;
  DISALLOW_COPY_AND_ASSIGN(ResourceLoader);
};
}  // namespace lord
