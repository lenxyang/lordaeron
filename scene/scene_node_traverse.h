#pragma once

#include "base/basictypes.h"

namespace lord {

class SceneNode;

class SceneNodeTraverseDelegate {
 public:
  SceneNodeTraverseDelegate() {}
  virtual ~SceneNodeTraverseDelegate() {}

  virtual void OnTraverseBegin(SceneNode* root) = 0;
  virtual bool OnTraverseNodeEnter(SceneNode* node) = 0;
  virtual void OnTraverseNodeExit(SceneNode* node) = 0;
  virtual void OnTraverseEnd() = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(SceneNodeTraverseDelegate);
};

class SceneNodeTraverse {
 public:
  SceneNodeTraverse(SceneNodeTraverseDelegate* delegate);
  ~SceneNodeTraverse();

  void Traverse(SceneNode* node);
 private:
  void TraverseNode(SceneNode* node);
  SceneNodeTraverseDelegate* delegate_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeTraverse);
};
}  // namespace lord
