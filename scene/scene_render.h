#pragma once

#include "base/memory/scoped_ptr.h"
#include "azer/render/render.h"
#include "lordaeron/scene/scene_node_traverse.h"

namespace lord {
class SceneContext;
class SceneNode;
class SceneEnvironment;

class NodeRendererTraverseDelegate : public SceneNodeTraverseDelegate {
 public:
  NodeRendererTraverseDelegate(SceneEnvironment* env);
  ~NodeRendererTraverseDelegate();

  void OnTraverseBegin(SceneNode* root) override;
  bool OnTraverseNodeEnter(SceneNode* node) override;
  void OnTraverseNodeExit(SceneNode* node) override;
  void OnTraverseEnd() override;
 private:
  void UpdateNodeWorld(SceneNode* node);
  SceneEnvironment* environment_;
  DISALLOW_COPY_AND_ASSIGN(NodeRendererTraverseDelegate);
};

class SceneRender {
 public:
  SceneRender(SceneContext* context, SceneNode* root);
  ~SceneRender();

  void Update(const azer::FrameArgs& args);
  void Draw(azer::Renderer* renderer);
 private:
  SceneContext* context_;
  SceneNode* root_;
  DISALLOW_COPY_AND_ASSIGN(SceneRender);
};
}  // namespace lord
