#include "lordaeron/resource/scene_loader.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "azer/base/file_system.h"
#include "lordaeron/context.h"
#include "lordaeron/util/xml_util.h"
#include "lordaeron/resource/resource_util.h"

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
  if (type_name == "environment") {
    node->SetNodeType(kEnvSceneNode);
  } else {
    ConfigNodes nodes = std::move(config->GetTaggedChildren("refer"));
    CHECK(nodes.size() == 0 || nodes.size() == 1u) 
        << "scene node[" << config->GetNodePath() << "] take multiple referred node";
    ConfigNode* cnode = nodes[0];
    int node_type = GetTypeFromString(cnode->GetAttr("type"));
    if (node_type == kResTypeMesh) {
      MeshPtr mesh = LoadReferMesh(cnode, ctx);
      if (!mesh.get()) return false;
      mesh->SetEffectAdapterContext(env->GetEffectAdapterContext());
      node->mutable_data()->AttachMesh(mesh);
    } else if (node_type == kResTypeLight) {
      LightPtr light = LoadReferLight(cnode, ctx);
      node->mutable_data()->AttachLight(light);
    } else {
      CHECK(false) << "not support type: " << type_name;
      return false;
    }
  }

  return true;
}
}  // namespace lord
