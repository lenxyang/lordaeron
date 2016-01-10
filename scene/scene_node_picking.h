#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node_traverse.h"

namespace lord {
class SceneNode;
class SceneNodePickHelper : public SceneNodeTraverseDelegate {
 public:
  SceneNodePickHelper(azer::Ray* ray);
  ~SceneNodePickHelper();

  SceneNode* GetPickingNode();
 protected:
  void OnTraverseBegin(SceneNode* root) override;
  bool OnTraverseNodeEnter(SceneNode* node) override;
  void OnTraverseNodeExit(SceneNode* node) override;
  void OnTraverseEnd() override;
 private:
  const azer::Ray* ray_;
  SceneNode* picking_node_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodePickHelper);
};
}  // namespace lord
