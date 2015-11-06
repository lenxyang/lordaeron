#pragma once

#include "azer/base/config_node.h"
#include "lordaeron/render/light.h"
#include "lordaeron/scene/scene_loader.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
//  <node name=" type="light">
//    <location>..</location>
//    <light type="point_light">
//      <ambient></ambient>
//      <diffuse></diffuse>
//      <specular></specular>
//      <range></range>
//    </light>
//  </node>
//
class LightNodeLoader : public SceneNodeLoader {
 public:
  LightNodeLoader();
  ~LightNodeLoader();

  const char* node_type_name() const override;
  bool LoadSceneNode(SceneNode* node, azer::ConfigNode* config) override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LightNodeLoader);
};
}  // namespace lord
