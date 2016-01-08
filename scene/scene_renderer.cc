#include "lordaeron/scene/scene_renderer.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/render_env_node.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using namespace azer;
SceneRenderer::SceneRenderer() : camera_(NULL) {
}

void SceneRenderer::SetTreeBuildDelegate(scoped_ptr<RenderTreeBuilderDelegate> delegate) {
  delegate_ = delegate.Pass();
}

void SceneRenderer::Init(SceneNode* root, const Camera* camera) {
  DCHECK(delegate_.get());
  CHECK(root_ == NULL);
  camera_ = camera;
  RenderTreeBuilder builder(delegate_.get());
  root_ = builder.Build(root, camera);
}

void SceneRenderer::Update(const FrameArgs& args) {
  root_->GetEnvNode()->UpdateRecusive(args);
  OnFrameUpdateBegin(args);
  UpdateNodeRecusive(root_, args);
  OnFrameUpdateEnd(args);
}

void SceneRenderer::Render(Renderer* renderer) {
  OnFrameRenderBegin(renderer);
  RenderNodeRecusive(root_, renderer);
  OnFrameRenderEnd(renderer);
}

bool SceneRenderer::OnUpdateNode(RenderNode* node, const FrameArgs& args) {
  if (!node->GetSceneNode()->visible()) {
    return false;
  }
  node->Update(args);
  return true;
}

bool SceneRenderer::OnRenderNode(RenderNode* node, Renderer* renderer) {
  if (!node->GetSceneNode()->visible()) {
    return false;
  }

  node->Render(renderer);
  return true;
}

void SceneRenderer::UpdateNodeRecusive(RenderNode* node, const FrameArgs& args) {
  if (OnUpdateNode(node, args)) {
    for (auto iter = node->children().begin(); 
         iter != node->children().end(); ++iter) {
      UpdateNodeRecusive(iter->get(), args);
    }
  }
}

void SceneRenderer::RenderNodeRecusive(RenderNode* node, Renderer* renderer) {
  if (OnRenderNode(node, renderer)) {
    for (auto iter = node->children().begin(); 
         iter != node->children().end(); ++iter) {
      RenderNodeRecusive(iter->get(), renderer);
    }
  }
}
}  // namespace lord
