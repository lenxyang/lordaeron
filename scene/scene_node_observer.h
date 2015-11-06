#pragma once

#include "base/basictypes.h"
#include "azer/render/render.h"

namespace lord {

class SceneNode;
class SceneNodeObserver {
 public:
  virtual void OnSceneNodeLocationChanged(SceneNode* node,
                                          const azer::Vector3& prevpos) {
  }

  virtual void OnSceneNodeOrientationChanged(SceneNode* node, 
                                             const azer::Quaternion& prev_orient) {}

  virtual void OnSceneNodeBoundsChanged(SceneNode* node, 
                                        const azer::Vector3& orgmin,
                                        const azer::Vector3& orgmax) {}
};
}  // namespace lord
