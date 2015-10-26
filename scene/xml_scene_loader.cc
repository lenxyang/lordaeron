#include "lordaeron/scene/xml_scene_loader.h"

#include "lordaeron/scene/scene_node_data.h"

namespace lord {
XMLSceneLoader::XMLSceneLoader() {
}

XMLSceneLoader::~XMLSceneLoader() {
}

SceneNodePtr XMLSceneLoader::Load(const std::string& contents) {
  return SceneNodePtr();
}
}  // namespace lord
