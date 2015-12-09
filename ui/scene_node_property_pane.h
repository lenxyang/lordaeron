#pragma once

#include "nelf/nelf.h"

namespace lord {
class SceneNode;
class SceneNodePropertyPane : public nelf::CollapseView {
 public:
  SceneNodePropertyPane();

  void SetSceneNode(SceneNode* node);
 private:
  void ClearUI();
  void InitUIForLampNode();
  void InitUIForObjectNode();
  void InitUIForCommonNode();
  SceneNode* node_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodePropertyPane);
};
}  // namespace lord
