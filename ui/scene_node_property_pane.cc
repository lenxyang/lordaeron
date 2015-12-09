#include "lordaeron/ui/scene_node_property_pane.h"

#include "base/logging.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/light_pane.h"

namespace lord {
SceneNodePropertyPane::SceneNodePropertyPane() 
    : node_(NULL) {
}

void SceneNodePropertyPane::ClearUI() {
  RemoveAllChildViews(true);
}

void SceneNodePropertyPane::SetSceneNode(SceneNode* node) {
  ClearUI();
  node_ = node;
  InitUIForCommonNode();
  switch (node->type()) {
    case kLampSceneNode:
      InitUIForLampNode();
      break;
    case kObjectSceneNode:
      InitUIForObjectNode();
      break;
    default:
      break;
  }
}

void SceneNodePropertyPane::InitUIForLampNode() {
}

void SceneNodePropertyPane::InitUIForObjectNode() {
}

void SceneNodePropertyPane::InitCommonNodeUI() {
}
}  // namespace lord
