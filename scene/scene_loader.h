#pragma once

#include "azer/base/config_node.h"
#include "azer/base/file_system.h"
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
class SceneLoader;

struct SceneLoadContext {
  SceneLoader* loader;
  azer::FileSystem* filesystem;
  azer::ResPath path;
};

class SceneNodeLoader {
 public:
  virtual const char* node_type_name() const = 0;
  virtual bool LoadSceneNode(SceneNode* node, azer::ConfigNode* config, 
                             SceneLoadContext* ctx) = 0;
};

// must be stateless class
class SceneLoader {
 public:
  SceneLoader(azer::FileSystem* fs, SceneContext* context);
  ~SceneLoader();

  SceneNodeLoader* GetLoader(const std::string& name);
  void RegisterSceneNodeLoader(scoped_ptr<SceneNodeLoader> loader);

  // Load Scene
  SceneNodePtr Load(const azer::ResPath& path, const std::string& nodepath);
 private:
  bool InitSceneNodeRecusive(SceneNode* node, azer::ConfigNode* config_node,
                             SceneLoadContext* ctx);
  bool InitSceneNode(SceneNode* node, azer::ConfigNode* config, 
                     SceneLoadContext* ctx);
  bool LoadChildrenNode(SceneNode* node, azer::ConfigNode* config,
                        SceneLoadContext* ctx);
  bool LoadSceneLocation(SceneNode* node, azer::ConfigNode* config,
                         SceneLoadContext* ctx);

  std::map<std::string, scoped_ptr<SceneNodeLoader> >loader_map_;
  azer::FileSystem* filesystem_;
  SceneContext* scene_context_;
  DISALLOW_COPY_AND_ASSIGN(SceneLoader);
};
}
