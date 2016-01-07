#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {

class SceneRenderer {
 public:
  SceneRenderer();
  SceneRenderNode* root() { return root_.get();}

  const azer::Camera* camera() const { return camera_;}
  void SetDelegateFactory(scoped_ptr<SceneNodeRenderDelegateFactory> factory);
  void Init(SceneNode* root, const azer::Camera* camera);
  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);

  virtual void OnFrameUpdateBegin(const azer::FrameArgs& args) {}
  virtual void OnFrameRenderBegin(azer::Renderer* renderer) {}
  virtual bool UpdateNode(SceneRenderNode* node, const azer::FrameArgs& args);
  virtual bool RenderNode(SceneRenderNode* node, azer::Renderer* renderer);
  virtual void OnFrameUpdateEnd(const azer::FrameArgs& args) {}
  virtual void OnFrameRenderEnd(azer::Renderer* renderer) {}
 protected:
  void UpdateNodeRecusive(SceneRenderNode* node, 
                          const azer::FrameArgs& args);
  void RenderNodeRecusive(SceneRenderNode* node, azer::Renderer* renderer);
  SceneRenderNodePtr root_;
  scoped_ptr<SceneNodeRenderDelegateFactory> factory_;
  const azer::Camera* camera_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderer);
};
}
