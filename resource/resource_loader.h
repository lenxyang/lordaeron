#pragma once

#include <map>
#include <string>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "azer/base/repository_node.h"
#include "azer/base/config_node.h"
#include "azer/base/file_system.h"
#include "lordaeron/resource/resource.h"

namespace lord {
class ResourceLoader;
class ResourceSpecialLoader;
typedef scoped_refptr<ResourceSpecialLoader> ResourceSpecialLoaderPtr;

struct ResourceLoaderContext {
  ResourceLoader* loader;
  azer::ResPath path;
  azer::RepositoryNodePtr* root;
  azer::FileSystem* filesystem;
};

class ResourceSpecialLoader : public ::base::RefCounted<ResourceSpecialLoader> {
 public:
  virtual const char* GetLoaderName() const = 0;
  virtual bool CouldLoad(azer::ConfigNode* node) const = 0;
  virtual Resource Load(const azer::ConfigNode* node, ResourceLoaderContext* ctx) = 0;
};

class ResourceLoader {
 public:
  explicit ResourceLoader(azer::FileSystem* fs);
  ~ResourceLoader();

  void RegisterSpecialLoader(ResourceSpecialLoader* loader);
  ResourceSpecialLoader* GetLoader(azer::ConfigNode *node);

  Resource Load(const azer::ResPath& path);
 private:
  std::map<std::string, ResourceSpecialLoaderPtr> dict_;
  azer::FileSystem* filesystem_;
  DISALLOW_COPY_AND_ASSIGN(ResourceLoader);
};

azer::EffectPtr LoadReferEffect(const azer::ConfigNode* node,
                                ResourceLoaderContext* ctx);
azer::MeshPtr LoadReferMesh(const azer::ConfigNode* node, ResourceLoaderContext* ctx);
Material LoadReferMaterial(const azer::ConfigNode* node, ResourceLoaderContext* ctx);
Resource LoadReferResource(const azer::ResPath& path, int type,
                      ResourceLoaderContext* ctx);
Resource LoadResource(const azer::ResPath& path, int type,
                      ResourceLoaderContext* ctx);
Resource LoadReferResource(const azer::ConfigNode* node, ResourceLoaderContext* ctx);

bool Repath(const azer::ResPath& path, azer::ResPath* apath,
            ResourceLoaderContext* ctx);
void InitDefaultLoader(ResourceLoader* loder);
}  // namespace lord
