#include "lordaeron/resource/resource_loader.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "azer/base/file_system.h"
#include "lordaeron/util/xml_util.h"
#include "lordaeron/resource/mesh_loader.h"
#include "lordaeron/resource/light_loader.h"
#include "lordaeron/resource/vertex_desc_loader.h"
#include "lordaeron/resource/scene_loader.h"

namespace lord {
using namespace azer;
namespace {
bool LoadContents(const ResPath& path, FileContents* contents, FileSystem* fs) {
  FilePtr file = fs->OpenFile(path);
  if (!file.get()) {
    LOG(ERROR) << "Failed to open file: " << path.fullpath();
    return false;
  }

  if (!file->PRead(0, -1, contents)) {
    LOG(ERROR) << "Failed to read file: " << path.fullpath();
    return false;
  }
  return true;
}
}

ResourceLoader::ResourceLoader(azer::FileSystem* fs)
    : filesystem_(fs) {
}

ResourceLoader::~ResourceLoader() {
}

void ResourceLoader::RegisterSpecialLoader(ResourceSpecialLoader* loader){
  auto iter = dict_.find(loader->GetLoaderName());
  if (iter == dict_.end()) {
    dict_.insert(std::make_pair(loader->GetLoaderName(), loader));
  } else {
    CHECK(false) << "Loader \"" << loader->GetLoaderName() << "\" has been inserted";
  }
}

ResourceSpecialLoader* ResourceLoader::GetLoader(azer::ConfigNode *node) {
  for (auto iter = dict_.begin(); iter != dict_.end(); ++iter) {
    if (iter->second->CouldLoad(node)) {
      return iter->second.get();
    }
  }
  return NULL;
}

Resource ResourceLoader::Load(const azer::ResPath& path) {
  FileContents contents;
  if (!LoadContents(path, &contents, filesystem_)) {
    LOG(ERROR) << "Failed to Load contents from file: " << path.fullpath();
    return Resource();
  }

  ResourceLoaderContext ctx;
  ctx.loader = this;
  ctx.filesystem = filesystem_;
  std::string strcontents((const char*)&contents.front(), contents.size());
  ConfigNodePtr croot = ConfigNode::InitFromXMLStr(strcontents);

  base::string16 nodepath = ::base::UTF8ToUTF16("//");
  nodepath.append(path.component_name().as_string());
  ConfigNodePtr cnode = croot->GetNodeFromPath(::base::UTF16ToUTF8(nodepath));
  ResourceSpecialLoader* loader = GetLoader(cnode);
  return loader->Load(cnode.get(), &ctx);
}

void InitDefaultLoader(ResourceLoader* loader) {
  loader->RegisterSpecialLoader(new MeshLoader);
  loader->RegisterSpecialLoader(new VertexDescLoader);
  loader->RegisterSpecialLoader(new LightLoader);
  loader->RegisterSpecialLoader(new SceneLoader);
}
}  // namespace lord
