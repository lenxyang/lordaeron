#pragma once

#include "azer/base/config_node.h"
#include "azer/render/render.h"
#include "lordaeron/render/light.h"
#include "lordaeron/scene/scene_node.h"

// xml
// <scene name="scene_name">
//   <node name="" type="lamp">
//     <lamp type="directional">
//       <params>
//       <params>
//     </lamp>
//   </node>
//   <node name="" type="mesh">
//     <location>
//       <position></position>
//       <orientation></orientation>
//     </location>
//     <mesh>
//      <effect></effect>
//      <params>
//      </params>
//      <path></path>
//     </mesh>
//   </node>
// </scene>

// <location>
//   <position></position>
// </location>

// <lamp></lamp>
// <mesh></mesh>
// <terrain></terrain>
// <water></water>
// <bounding_volumn></bounding_volumn>

namespace lord {
class SceneNode;
class SceneNodeData;

class SceneNodeLoader {
 public:
  virtual const char* node_type_name() const = 0;
  virtual bool LoadSceneNode(SceneNode* node, azer::ConfigNode* config) = 0;
};

class SceneLoader {
 public:
  explicit SceneLoader();
  ~SceneLoader();

  SceneNodeLoader* GetLoader(const std::string& name);
  void RegisterSceneNodeLoader(scoped_ptr<SceneNodeLoader> loader);

  // Load Scene
  bool Load(SceneNode* root, azer::ConfigNode* config_root);
 private:
  bool InitSceneNodeRecusive(SceneNode* node, azer::ConfigNode* config_node);
  bool InitSceneNode(SceneNode* node, azer::ConfigNode* config);
  bool LoadChildrenNode(SceneNode* node, azer::ConfigNode* config);
  bool LoadSceneLocation(SceneNode* node, azer::ConfigNode* config);

  std::map<std::string, scoped_ptr<SceneNodeLoader> >loader_map_;
  DISALLOW_COPY_AND_ASSIGN(SceneLoader);
};
}
