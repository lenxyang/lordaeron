#pragma once

#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"
#include "azer/render/render.h"

namespace lord {
class RenderTreeBuilderDelegate;
class RenderNode;
class RenderEnvNode;
typedef scoped_refptr<RenderNode> RenderNodePtr;
class SceneNode;
class SceneRender;

class SceneRenderObserver {
 public:
  virtual void OnFrameUpdateBegin(SceneRender* sr, const azer::FrameArgs& args) {}
  virtual void OnFrameUpdateEnd(SceneRender* sr, const azer::FrameArgs& args) {}
  virtual void OnFrameRenderBegin(SceneRender* sr, azer::Renderer* renderer) {}
  virtual void OnFrameRenderEnd(SceneRender* sr, azer::Renderer* renderer) {}
};

class SceneRender {
 public:
  SceneRender();

  RenderNode* root() { return root_.get();}
  const azer::Camera* camera() const { return camera_;}
  void SetTreeBuildDelegate(scoped_ptr<RenderTreeBuilderDelegate> delegate);
  void Init(SceneNode* root, const azer::Camera* camera);
  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);

  void AddObserver(SceneRenderObserver* observer);
  void RemoveObserver(SceneRenderObserver* observer);
  bool HasObserver(SceneRenderObserver* observer) const;

  virtual void OnFrameUpdateBegin(const azer::FrameArgs& args) {};
  virtual bool OnUpdateNode(RenderNode* node, const azer::FrameArgs& args);
  virtual void OnFrameUpdateEnd(const azer::FrameArgs& args) {};

  virtual void OnFrameRenderBegin(azer::Renderer* renderer) {};
  virtual bool OnRenderNode(RenderNode* node, azer::Renderer* renderer);
  virtual void OnFrameRenderEnd(azer::Renderer* renderer) {};
 protected:
  
  void DoFrameUpdateBegin(const azer::FrameArgs& args);
  void DoFrameUpdateEnd(const azer::FrameArgs& args);
  void DoFrameRenderBegin(azer::Renderer* renderer);
  void DoFrameRenderEnd(azer::Renderer* renderer);

  void UpdateNodeRecusive(lord::RenderNode* node, const azer::FrameArgs& args);
  void RenderNodeRecusive(lord::RenderNode* node, azer::Renderer* renderer);
  RenderNodePtr root_;
  scoped_ptr<RenderTreeBuilderDelegate> delegate_;
  const azer::Camera* camera_;
  ObserverList<SceneRenderObserver> observers_;
  DISALLOW_COPY_AND_ASSIGN(SceneRender);
};
}
