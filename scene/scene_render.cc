#include "lordaeron/scene/scene_render.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/render_env_node.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using namespace azer;
SceneRender::SceneRender() : camera_(NULL) {
  render_state_ = RenderSystem::Current()->CreateRenderState();
  render_state_->EnableDepthTest(true);
}

void SceneRender::SetTreeBuildDelegate(scoped_ptr<RenderTreeBuilderDelegate> delegate) {
  delegate_ = delegate.Pass();
}

void SceneRender::Init(SceneNode* root, const Camera* camera) {
  DCHECK(delegate_.get());
  CHECK(root_ == NULL);
  camera_ = camera;
  RenderTreeBuilder builder(delegate_.get());
  root_ = builder.Build(root, camera);
}

void SceneRender::Update(const FrameArgs& args) {
  root_->GetEnvNode()->UpdateRecusive(args);
  DoFrameUpdateBegin(args);
  UpdateNodeRecusive(root_, args);
  DoFrameUpdateEnd(args);
}

void SceneRender::Render(Renderer* renderer) {
  DoFrameRenderBegin(renderer);
  ScopedRenderState scoped_render_state(renderer, render_state_);
  RenderNodeRecusive(root_, renderer);
  DoFrameRenderEnd(renderer);
}

bool SceneRender::OnUpdateNode(RenderNode* node, const FrameArgs& args) {
  if (!node->GetSceneNode()->visible()) {
    return false;
  }
  node->Update(args);
  return true;
}

bool SceneRender::OnRenderNode(RenderNode* node, Renderer* renderer) {
  if (!node->GetSceneNode()->visible()) {
    return false;
  }

  node->Render(renderer);
  return true;
}

void SceneRender::UpdateNodeRecusive(RenderNode* node, const FrameArgs& args) {
  if (OnUpdateNode(node, args)) {
    for (auto iter = node->children().begin(); 
         iter != node->children().end(); ++iter) {
      UpdateNodeRecusive(iter->get(), args);
    }
  }
}

void SceneRender::RenderNodeRecusive(RenderNode* node, Renderer* renderer) {
  if (OnRenderNode(node, renderer)) {
    for (auto iter = node->children().begin(); 
         iter != node->children().end(); ++iter) {
      RenderNodeRecusive(iter->get(), renderer);
    }
  }
}

void SceneRender::AddObserver(SceneRenderObserver* observer) {
  observers_.AddObserver(observer);
}

void SceneRender::RemoveObserver(SceneRenderObserver* observer) {
  observers_.RemoveObserver(observer);
}

bool SceneRender::HasObserver(SceneRenderObserver* observer) const {
  return observers_.HasObserver(observer);
}

void SceneRender::DoFrameUpdateBegin(const FrameArgs& args) {
  OnFrameUpdateBegin(args);
  FOR_EACH_OBSERVER(SceneRenderObserver, 
                    observers_, 
                    OnFrameUpdateBegin(this, args));
}

void SceneRender::DoFrameUpdateEnd(const FrameArgs& args) {
  OnFrameUpdateEnd(args);
  FOR_EACH_OBSERVER(SceneRenderObserver, 
                    observers_, 
                    OnFrameUpdateEnd(this, args));
}

void SceneRender::DoFrameRenderBegin(Renderer* renderer) {
  OnFrameRenderBegin(renderer);
  FOR_EACH_OBSERVER(SceneRenderObserver, 
                    observers_, 
                    OnFrameRenderBegin(this, renderer));
}

void SceneRender::DoFrameRenderEnd(Renderer* renderer) {
  OnFrameRenderEnd(renderer);
  FOR_EACH_OBSERVER(SceneRenderObserver, 
                    observers_, 
                    OnFrameRenderEnd(this, renderer));
}
}  // namespace lord
