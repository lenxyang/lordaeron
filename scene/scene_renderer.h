#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {
class SceneNodeRenderDelegateFactory {
 public:
  virtual scoped_ptr<SceneRenderNodeDelegate> CreateDelegate(SceneRenderNode* n) = 0;
};

class SceneRenderTreeBuilder : public SceneNodeTraverseDelegate {
 public:
  explicit SceneRenderTreeBuilder(SceneNodeRenderDelegateFactory* factory);
  ~SceneRenderTreeBuilder();

  SceneRenderNodePtr Build(SceneNode* node, const azer::Camera* camera);

  // override from SceneNodeTraverseDelegate
  void OnTraverseBegin(SceneNode* root) override;
  bool OnTraverseNodeEnter(SceneNode* node) override;
  void OnTraverseNodeExit(SceneNode* node) override;
  void OnTraverseEnd() override;
 private:
  void UpdateNodeWorld(SceneNode* node);
  SceneRenderNode* cur_;
  SceneNodeRenderDelegateFactory* factory_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderTreeBuilder);
};

class SceneRenderer {
 public:
  SceneRenderer();
  SceneRenderNode* root() { return root_.get();}

  void SetDelegateFactory(scoped_ptr<SceneNodeRenderDelegateFactory> factory);
  void Init(SceneNode* root, const azer::Camera* camera);
  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);

  virtual void OnFrameUpdateBegin(const azer::FrameArgs& args) {}
  virtual void OnFrameRenderBegin(azer::Renderer* renderer) {}
  virtual bool UpdateNode(SceneRenderNode* node, const azer::FrameArgs& args) = 0;
  virtual bool RenderNode(SceneRenderNode* node, azer::Renderer* renderer) = 0;
  virtual void OnFrameUpdateEnd(const azer::FrameArgs& args) {}
  virtual void OnFrameRenderEnd(azer::Renderer* renderer) {}
 protected:
  void UpdateNodeRecusive(SceneRenderNode* node, 
                          const azer::FrameArgs& args);
  void RenderNodeRecusive(SceneRenderNode* node, azer::Renderer* renderer);
  SceneRenderNodePtr root_;
  scoped_ptr<SceneNodeRenderDelegateFactory> factory_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderer);
};
}
