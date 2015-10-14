#pragma once

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/render/light.h"

namespace lord {
class SceneNodeData {
 public:
  SceneNodeData(SceneNode* node);
  ~SceneNodeData();

  SceneNode::Type type() const { return type_;}

  void reset();

  void AttachMesh(azer::MeshPtr mesh);
  azer::MeshPtr GetMesh();

  void AttachLight(LightPtr light);
  LightPtr light() { return light_;}

  void SetSceneNode(SceneNode* node);
 private:
  SceneNode::Type type_;

  azer::MeshPtr mesh_;
  LightPtr light_;
  SceneNode* node_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeData);
};

typedef scoped_refptr<SceneNodeData> SceneNodeDataPtr;
  
}  // namespace lord
