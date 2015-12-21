#pragma once

#include "azer/base/config_node.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/resource/scene_loader.h"
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
  bool LoadSceneNode(SceneNode* node, const azer::ConfigNode* config,
                     SceneLoadContext* ctx) override;
 private:
  bool LoadAttenuation(Attenuation* atten, azer::ConfigNode* config);
  DISALLOW_COPY_AND_ASSIGN(LightNodeLoader);
};

class EnvNodeLoader : public SceneNodeLoader {
 public:
  static const char kNodeTypeName[];
  EnvNodeLoader();
  ~EnvNodeLoader();

  const char* node_type_name() const override;
  bool LoadSceneNode(SceneNode* node, const azer::ConfigNode* config,
                     SceneLoadContext* ctx) override;
 private:
  DISALLOW_COPY_AND_ASSIGN(EnvNodeLoader);
};

}  // namespace lord
