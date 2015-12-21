#pragma once

#include "azer/base/config_node.h"
#include "azer/base/file_system.h"
#include "azer/render/render.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/resource/resource_loader.h"

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
//      <effect path="//"/>
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
class SceneLoader;

class SceneNodeLoader {
 public:
  virtual const char* node_type_name() const = 0;
  virtual bool LoadSceneNode(SceneNode* node, const azer::ConfigNode* config, 
                             ResourceLoaderContext* ctx) = 0;
};

// must be stateless class
class SceneLoader : public ResourceSpecialLoader {
 public:
  static const char kSpecialLoaderName[];
  SceneLoader();
  ~SceneLoader();

  SceneNodeLoader* GetLoader(const std::string& name);
  void RegisterSceneNodeLoader(scoped_ptr<SceneNodeLoader> loader);

  const char* GetLoaderName() const override;
  Resource Load(const azer::ConfigNode* node, ResourceLoaderContext* ctx) override;
  bool CouldLoad(azer::ConfigNode* node) const override;

  // Load Scene
  SceneNodePtr LoadNode(const azer::ConfigNode* node, ResourceLoaderContext* ctx);
 private:
  bool InitSceneNodeRecusive(SceneNode* node, const azer::ConfigNode* config_node,
                             ResourceLoaderContext* ctx);
  bool InitSceneNode(SceneNode* node, const azer::ConfigNode* config, 
                     ResourceLoaderContext* ctx);
  bool LoadChildrenNode(SceneNode* node, const azer::ConfigNode* config,
                        ResourceLoaderContext* ctx);
  bool LoadSceneLocation(SceneNode* node, const azer::ConfigNode* config,
                         ResourceLoaderContext* ctx);

  std::map<std::string, scoped_ptr<SceneNodeLoader> >loader_map_;
  DISALLOW_COPY_AND_ASSIGN(SceneLoader);
};
}
