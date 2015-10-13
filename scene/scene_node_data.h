#pragma once

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/render/light.h"

namespace lord {
class SceneNodeData : public ::base::RefCounted<SceneNodeData> {
 public:
  SceneNodeData(SceneNode::Type type);
  ~SceneNodeData();

  SceneNode::Type type() const { return type_;}

  void reset();

  void AttachMesh(azer::MeshPtr mesh);
  azer::MeshPtr GetMesh();
 private:
  SceneNode::Type type_;

  azer::MeshPtr mesh_;
  LightPtr light_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeData);
};

typedef scoped_refptr<SceneNodeData> SceneNodeDataPtr;
  
}  // namespace lord
