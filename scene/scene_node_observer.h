#pragma once

#include "base/basictypes.h"
#include "azer/render/render.h"

namespace lord {

class SceneNode;
class SceneNodeObserver {
 public:
  virtual void OnSceneNodeLocationChanged(SceneNode* node, azer::Vector3& prevpos){
  }

  virtual void OnSceneNodeOrientationChanged(SceneNode* node, 
                                             azer::Quaternion& prev_orient) {
  }

  virtual void OnSceneNodeBoundingChanged(SceneNode* node) {
  }
};
}  // namespace lord
