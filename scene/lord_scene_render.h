#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node_traverse.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {
class SceneNode;

class LordObjectNodeRenderDelegate : public SceneRenderNodeDelegate {
 public:
  explicit LordObjectNodeRenderDelegate(SceneRenderNode* node);
  bool Init() override;
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  azer::MeshPtr mesh_;
  DISALLOW_COPY_AND_ASSIGN(LordObjectNodeRenderDelegate);
};

class LordLampNodeRenderDelegate : public SceneRenderNodeDelegate {
 public:
  explicit LordLampNodeRenderDelegate(SceneRenderNode* node);
  bool Init() override;
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  azer::MeshPtr mesh_;
  DISALLOW_COPY_AND_ASSIGN(LordLampNodeRenderDelegate);
};

class LoadSceneRenderNodeDelegateFactory : public SceneRenderNodeDelegateFactory {
 public:
  scoped_ptr<SceneRenderNodeDelegate> CreateDelegate(SceneRenderNode* node) override;
};

class SimpleRenderTreeRenderer {
 public:
  explicit SimpleRenderTreeRenderer(SceneRenderNode* root);
  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);
 private:
  void UpdateNode(SceneRenderNode* node, const azer::FrameArgs& args);
  void RenderNode(SceneRenderNode* node, azer::Renderer* renderer);
  SceneRenderNode* root_;
  std::vector<SceneRenderNode*> blending_node_;
  DISALLOW_COPY_AND_ASSIGN(SimpleRenderTreeRenderer);
};

azer::MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node);
}  // namespace lord
