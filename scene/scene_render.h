#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node_traverse.h"

namespace lord {
class SceneEnvironment;
typedef scoped_refptr<SceneEnvironment> SceneEnvironmentPtr;

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

class SceneRenderContext {
 public:
  SceneRenderContext();
  ~SceneRenderContext();

  void Update(const azer::FrameArgs& args);
  void Draw(azer::Renderer* renderer);
 private:
  SceneEnvironmentPtr environment_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderContext);
};
}  // namespace lord
