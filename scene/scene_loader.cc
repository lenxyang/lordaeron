#include "lordaeron/scene/scene_loader.h"

#include "base/logging.h"
#include "lordaeron/util/xml_util.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
using azer::ConfigNode;
using azer::ConfigNodePtr;

SceneLoader::SceneLoader(SceneLoaderDelegate* delegate)
    : delegate_(delegate) {
}

SceneLoader::~SceneLoader() {
}

bool SceneLoader::Load(SceneNode* root, ConfigNode* croot) {
  DCHECK(croot->tagname() == "scene");
  if (LoadChildrenNode(root, croot)) {
    return true;
  } else {
    return false;
  }
}

bool SceneLoader::LoadChildrenNode(SceneNode* node, azer::ConfigNode* config) {
  std::vector<ConfigNodePtr> subnodes = config->GetNamedChildren("node");
  for (auto iter = subnodes.begin(); iter != subnodes.end(); ++iter) {
    azer::ConfigNode* child_config = iter->get();
    SceneNodePtr child_node(new SceneNode(child_config->GetAttr("name")));
    node->AddChild(child_node);
    if (!InitSceneNodeRecusive(child_node, child_config)) {
      LOG(INFO) << "Failed to init childnode, parent[" << node->path() << "]";
      return false;
    }
  }
  
  return true;
}

bool SceneLoader::InitSceneNodeRecusive(SceneNode* node, ConfigNode* config_node) {
  if (!InitSceneNode(node, config_node)) {
    LOG(INFO) << "Failed to init childnode, parent[" << node->path() << "]";
    return false;
  }

  if (!LoadChildrenNode(node, config_node)) {
    return false;
  }

  return true;
}

bool SceneLoader::LoadSceneLocation(SceneNode* node, azer::ConfigNode* config) {
  std::vector<ConfigNodePtr> location_children = std::move(
      config->GetNamedChildren("location"));
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
  if (location->HasNamedChild("position")) {
    if (location->GetChildTextAsVec3("position", &position)) {
      node->SetPosition(position);
    } else {
      LOG(ERROR) << "Node[" << node->path() << "] Invalid location: " 
                 << location->GetChildTextString("position");
      return false;
    }
  }

  if (location->HasNamedChild("scale")) {
    if (location->GetChildTextAsVec3("scale", &scale)) {
      node->SetScale(scale);
    } else {
      LOG(ERROR) << "Node[" << node->path() << "]Invalid location: " 
                 << location->GetChildTextString("scale");
      return false;
    }
  }

  if (location->HasNamedChild("orientation")) {
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

bool SceneLoader::InitSceneNode(SceneNode* node, ConfigNode* config) {
  if (!LoadSceneLocation(node, config)) {
    return false;
  }

  return delegate_->InitSceneNode(node, config);
}
}  // namespace lord
