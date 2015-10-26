#pragma once

#include "lordaeron/scene/scene_node.h"

namespace lord {
class XMLSceneLoader {
 public:
  XMLSceneLoader();
  ~XMLSceneLoader();

  // Load Scene
  SceneNodePtr Load(const std::string& contents);
 private:
  DISALLOW_COPY_AND_ASSIGN(XMLSceneLoader);
};
}
