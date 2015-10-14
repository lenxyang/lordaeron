#include "lordaeron/scene/scene_node_traverse.h"

#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
SceneNodeTraverse::SceneNodeTraverse(SceneNodeTraverseDelegate* delegate) 
    : delegate_(delegate) {
}

void SceneNodeTraverse::Traverse(SceneNode* root) {
  delegate_->OnTraverseBegin(root);
  TraverseNode(root);
  delegate_->OnTraverseEnd();
}

void SceneNodeTraverse::TraverseNode(SceneNode* node) {
  if (delegate_->OnTraverseNodeEnter(node)) {
    for (int32 i = 0; i < node->child_count(); ++i) {
      SceneNodePtr child = node->child_at(i);
      TraverseNode(child.get());
    }

    delegate_->OnTraverseNodeExit(node);
  }
}
}  // namespace lord
