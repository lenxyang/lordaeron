#pragma once

#include "base/basictypes.h"
#include "azer/render/render.h"

namespace lord {

class SceneNode;
class SceneNodeObserver {
 public:
  virtual void OnNodeLocationChanged(SceneNode* n, const azer::Vector3& prev) {}
  virtual void OnNodeOrientChanged(SceneNode* n, const azer::Quaternion& prev) {}
  virtual void OnNodeScaleChanged(SceneNode* node, const azer::Vector3& prev) {}
  virtual void OnNodeBoundsChanged(SceneNode* node, const azer::Vector3& orgmin,
                                   const azer::Vector3& orgmax) {}
};
}  // namespace lord
