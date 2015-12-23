#include "lordaeron/resource/scene_loader.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "azer/base/file_system.h"
#include "lordaeron/context.h"
#include "lordaeron/util/xml_util.h"

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

const char SceneLoader::kSpecialLoaderName[] = "lord::SceneLoader";

SceneLoader::SceneLoader() {}
SceneLoader::~SceneLoader() {}

SceneNodeLoader* SceneLoader::GetLoader(const std::string& name) {
  auto iter = loader_map_.find(name);
  if (loader_map_.end() != iter) {
    return iter->second.get();
  } else {
    return NULL;
  }
}

const char* SceneLoader::GetLoaderName() const {
  return kSpecialLoaderName;
}

bool SceneLoader::CouldLoad(azer::ConfigNode* node) const {
  const std::string attr = std::move(node->GetAttr("type"));
  return node->tagname() == "node" && (attr.empty() || attr == "scene");
}

Resource SceneLoader::Load(const ConfigNode* node, ResourceLoaderContext* ctx) {
  Resource ret;
  ret.scene = LoadNode(node, ctx);
  ret.retcode = (ret.scene.get() != NULL) ? 0 : -1;
  return ret;
}

void SceneLoader::RegisterSceneNodeLoader(scoped_ptr<SceneNodeLoader> loader) {
  DCHECK(NULL == GetLoader(loader->node_type_name()));
  loader_map_.insert(std::make_pair(loader->node_type_name(), loader.Pass()));
}

SceneNodePtr SceneLoader::LoadNode(const ConfigNode* cnode, 
                                   ResourceLoaderContext* ctx) {
  SceneNodePtr root(new SceneNode);
  if (LoadChildrenNode(root, cnode, ctx)) {
    return root;
  } else {
    return SceneNodePtr();
  }
}

bool SceneLoader::LoadChildrenNode(SceneNode* node, const ConfigNode* config,
                                   ResourceLoaderContext* ctx) {
  ConfigNodes subnodes = config->GetTaggedChildren("node");
  for (auto iter = subnodes.begin(); iter != subnodes.end(); ++iter) {
    ConfigNode* child_config = iter->get();
    SceneNodePtr child_node(new SceneNode(child_config->GetAttr("name")));
    node->AddChild(child_node);
    if (!InitSceneNodeRecusive(child_node, child_config, ctx)) {
      LOG(INFO) << "Failed to init childnode, parent[" << node->path() << "]";
      return false;
    }
  }
  
  return true;
}

bool SceneLoader::InitSceneNodeRecusive(SceneNode* node,
                                        const ConfigNode* config_node,
                                        ResourceLoaderContext* ctx) {
  if (!InitSceneNode(node, config_node, ctx)) {
    return false;
  }

  if (!LoadChildrenNode(node, config_node, ctx)) {
    return false;
  }

  return true;
}

bool SceneLoader::LoadSceneLocation(SceneNode* node,
                                    const ConfigNode* config,
                                    ResourceLoaderContext* ctx) {
  std::vector<ConfigNodePtr> location_children = std::move(
      config->GetTaggedChildren("location"));
  int32 location_size = location_children.size();
  if (location_size == 0u) {
	return true;
  } else if (location_size != 1u) {
    LOG(ERROR) << "location cannot be multiple.";
    return false;
  } 

  using namespace azer;
  ConfigNode* location = location_children[0].get();
  Vector3 position;
  Vector3 scale;
  Quaternion orient;
  if (location->HasTaggedChild("position")) {
    if (location->GetChildTextAsVec3("position", &position)) {
      node->SetPosition(position);
    } else {
      LOG(ERROR) << "Node[" << node->path() << "] Invalid location: " 
                 << location->GetChildTextString("position");
      return false;
    }
  }

  if (location->HasTaggedChild("scale")) {
    if (location->GetChildTextAsVec3("scale", &scale)) {
      node->SetScale(scale);
    } else {
      LOG(ERROR) << "Node[" << node->path() << "]Invalid location: " 
                 << location->GetChildTextString("scale");
      return false;
    }
  }

  if (location->HasTaggedChild("orientation")) {
    if (location->GetChildTextAsQuaternion("orientation", &orient)) {
      node->set_orientation(orient);
    } else {
      LOG(ERROR) << "Node[" << node->path() << "]Invalid location: " 
                 << location->GetChildTextString("orientation");
      return false;
    }
  }

  return true;
}

bool SceneLoader::InitSceneNode(SceneNode* node,
                                const ConfigNode* config,
                                ResourceLoaderContext* ctx) {
  Context* env = Context::instance(); 
  if (!LoadSceneLocation(node, config, ctx)) {
    LOG(ERROR) << "Failed to load node location information.";
    return false;
  }

  const std::string& type_name = config->GetAttr("type");
  if (config->HasAttr("refpath")) {
    ResPath npath;
    ResPath refpath(::base::UTF8ToUTF16(config->GetAttr("refpath")));
    CHECK(!refpath.fullpath().empty());
    Repath(refpath, &npath, ctx);
    Resource ret = ctx->loader->Load(refpath);
    if (ret.retcode != 0) {
      LOG(ERROR) << "Failed to load node: \"" << refpath.fullpath() << "\"";
      return false;
    }
    switch (ret.type) {
      case kResTypeMesh:
        ret.mesh->SetEffectAdapterContext(env->GetEffectAdapterContext());
        node->mutable_data()->AttachMesh(ret.mesh);
        break;
      case kResTypeLight:
        node->mutable_data()->AttachLight(ret.light);
        break;
      default:
        CHECK(false) << "not support type: " << ret.type;
        break;
    }
    return true;
  }

  if (type_name == "environment") {
    node->SetNodeType(kEnvSceneNode);
  }
  return true;
}
}  // namespace lord
