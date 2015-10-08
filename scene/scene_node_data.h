#pragma once

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
class SceneNodeData : public ::base::RefCounted<SceneNodeData> {
 public:
  SceneNodeData(SceneNode::Type type);
  ~SceneNodeData();

  SceneNode::Type type() const { return type_;}
 private:
  SceneNode::Type type_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeData);
};

typedef scoped_refptr<SceneNodeData> SceneNodeDataPtr;
  
}  // namespace lord
