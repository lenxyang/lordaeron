#include "lordaeron/scene/scene_loader.h"

#include "base/logging.h"
#include "azer/base/file_system.h"
#include "lordaeron/util/xml_util.h"
#include "lordaeron/scene/scene_context.h"
#include "lordaeron/scene/scene_node_data.h"

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

SceneLoader::SceneLoader(FileSystem* fs, SceneContext* context)
    : filesystem_(fs),
      scene_context_(context) {
}

SceneLoader::~SceneLoader() {
}

SceneNodeLoader* SceneLoader::GetLoader(const std::string& name) {
  auto iter = loader_map_.find(name);
  if (loader_map_.end() != iter) {
    return iter->second.get();
  } else {
    return NULL;
  }
}

void SceneLoader::RegisterSceneNodeLoader(scoped_ptr<SceneNodeLoader> loader) {
  DCHECK(NULL == GetLoader(loader->node_type_name()));
  loader_map_.insert(std::make_pair(loader->node_type_name(), loader.Pass()));
}

SceneNodePtr SceneLoader::Load(const ResPath& path, const std::string& nodepath) {
  SceneLoadContext context;
  context.filesystem = filesystem_;
  context.path = path;
  context.loader = this;

  SceneNodePtr root(new SceneNode(scene_context_));
  FileContents contents;
  if (!LoadContents(path, &contents, filesystem_)) {
    LOG(ERROR) << "Failed to Load contents from file: " << path.fullpath();
    return SceneNodePtr();
  }
  std::string strcontents((const char*)&(contents[0]), contents.size());
  ConfigNodePtr croot = ConfigNode::InitFromXMLStr(strcontents);
  if (!croot.get()) {
    LOG(ERROR) << "Failed to Extract ConfigNodeTree from file: " << path.fullpath();
    return SceneNodePtr();
  }

  ConfigNodePtr cnode = croot->GetNodeFromPath(nodepath);
  if (!cnode.get()) {
    LOG(ERROR) << "Failed to get CNode from path: \"" << nodepath << "\"";
    return SceneNodePtr();
  }

  if (LoadChildrenNode(root, cnode, &context)) {
    return root;
  } else {
    return SceneNodePtr();
  }
}

bool SceneLoader::LoadChildrenNode(SceneNode* node, ConfigNode* config,
                                   SceneLoadContext* ctx) {
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

bool SceneLoader::InitSceneNodeRecusive(SceneNode* node, ConfigNode* config_node,
                                        SceneLoadContext* ctx) {
  if (!InitSceneNode(node, config_node, ctx)) {
    return false;
  }

  if (!LoadChildrenNode(node, config_node, ctx)) {
    return false;
  }

  return true;
}

bool SceneLoader::LoadSceneLocation(SceneNode* node, ConfigNode* config,
                                    SceneLoadContext* ctx) {
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

bool SceneLoader::InitSceneNode(SceneNode* node, ConfigNode* config,
                                SceneLoadContext* ctx) {
  if (!LoadSceneLocation(node, config, ctx)) {
    LOG(ERROR) << "Failed to load node location information.";
    return false;
  }

  const std::string& type_name = config->GetAttr("type");
  if (!type_name.empty()) {
    SceneNodeLoader* loader = GetLoader(type_name);
    DCHECK(loader) << "no loader for type: " << type_name;
    if (!loader->LoadSceneNode(node, config, ctx)) {
      LOG(INFO) << "Failed to init childnode, parent[" << node->path() << "]";
      return false;
    }
  }
  return true;
}
}  // namespace lord
