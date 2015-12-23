#include "lordaeron/resource/resource_loader.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "azer/base/file_system.h"
#include "lordaeron/util/xml_util.h"
#include "lordaeron/resource/effect_loader.h"
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

  CHECK(path.IsAbsolutePath());
  ResourceLoaderContext ctx;
  ctx.path = path;
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

bool Repath(const azer::ResPath& path, azer::ResPath* apath,
            ResourceLoaderContext* ctx) {
  CHECK(!path.empty());
  CHECK(!ctx->path.empty());
  if (path.IsAbsolutePath()) {
    *apath = path;
  } else if (!path.component().empty() && path.filepath().empty()) {
    *apath = ResPath(ctx->path.filepath().as_string());
    CHECK(apath->Append(ResPath(path.component().as_string())));
  } else {
    *apath = ResPath(ctx->path.filepath().as_string());
    CHECK(apath->Append(ResPath(path.fullpath())));
  }
  return !apath->empty();
}

void InitDefaultLoader(ResourceLoader* loader) {
  loader->RegisterSpecialLoader(new EffectLoader);
  loader->RegisterSpecialLoader(new MeshLoader);
  loader->RegisterSpecialLoader(new VertexDescLoader);
  loader->RegisterSpecialLoader(new LightLoader);
  loader->RegisterSpecialLoader(new SceneLoader);
}

MeshPtr LoadReferMesh(const ConfigNode* node, ResourceLoaderContext* ctx) {
  Resource ret = LoadReferResource(node, ctx);
  CHECK(ret.type == kResTypeMesh);
  return ret.effect;
}

EffectPtr LoadReferEffect(const ConfigNode* node, ResourceLoaderContext* ctx) {
  Resource ret = LoadReferResource(node, ctx);
  CHECK(ret.type == kResTypeEffect);
  return ret.effect;
}

VertexDescPtr LoadReferVertexDesc(const ConfigNode* node, 
                                  ResourceLoaderContext* ctx) {
  Resource ret = LoadReferResource(node, ctx);
  CHECK(ret.type == kResTypeVertexDesc);
  return ret.vertex_desc;
}

Material LoadReferMaterial(const ConfigNode& node, ResourceLoaderContext* ctx) {
  Resource ret = LoadReferResource(node, ctx);
  CHECK(ret.type == kResTypeMaterial);
  return ret.material;
}

Resource LoadResource(const ResPath& path, int type, ResourceLoaderContext* ctx) {
  CHECK(!path.empty());
  ResPath npath;
  CHECK(Repath(path, &npath, ctx));
  Resource ret = ctx->loader->Load(npath);
  if (ret.retcode != 0) {
    LOG(ERROR) << "Load Effect failed for path: " << npath.fullpath();
    return Resource();
  }
  if (ret.type != type) {
    LOG(ERROR) << "Not Effect for path: " << npath.fullpath();
    return Resource();
  }

  return ret;
}

int32 GetTypeFromString(const std::string& str) {
  if (str == "mesh") {
    return kResTypeMesh;
  } else if (str == "material") {
    return kResTypeMaterial;
  } else if (str == "effect") {
    return kResTypeMaterial;
  } else if (str == "light") {
    return kResTypeLight;
  } else if (str == "scene") {
    return kResTypeScene;
  } else if (str == "vertex_desc") {
    return kResTypeVertexDesc;
  } else {
    NOTREACHED() << "unknown type: " << str;
    return kResTypeNone;
  }
}

Resource LoadReferResource(const ConfigNode* node, ResourceLoaderContext* ctx) {
  DCHECK(node->tag_name() == "refer");
  ResPath path(::base::UTF8ToUTF16(node->GetAttr("path")));
  CHECK(!path.empty());
  int type = GetTypeFromString(node->GetAttr("type"));
  return LoadResource(path, type, ctx);
}
}  // namespace lord
