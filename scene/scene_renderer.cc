#include "lordaeron/scene/scene_renderer.h"

#include "base/logging.h"

namespace lord {
using namespace azer;
SceneRenderer::SceneRenderer() {
}

void SceneRenderer::SetDelegateFactory(
    scoped_ptr<SceneNodeRenderDelegateFactory> factory) {
  factory_ = factory.Pass();
}

void SceneRenderer::Init(SceneNode* root, const Camera* camera) {
  DCHECK(factory_.get());
  CHECK(root_ == NULL);
  SceneRenderTreeBuilder builder(factory_.get());
  root_ = builder.Build(root, camera);
}

void SceneRenderer::Update(const azer::FrameArgs& args) {
  OnFrameUpdateBegin(args);
  UpdateNodeRecusive(root_, args);
  OnFrameUpdateEnd(args);
}

void SceneRenderer::Render(azer::Renderer* renderer) {
  OnFrameRenderBegin(renderer);
  RenderNodeRecusive(root_, renderer);
  OnFrameRenderEnd(renderer);
}

void SceneRenderer::UpdateNodeRecusive(SceneRenderNode* node, 
                                       const FrameArgs& args) {
  if (UpdateNode(node, args)) {
    for (auto iter = node->children().begin(); 
         iter != node->children().end(); ++iter) {
      UpdateNodeRecusive(iter->get(), args);
    }
  }
}

void SceneRenderer::RenderNodeRecusive(SceneRenderNode* node, Renderer* renderer) {
  if (RenderNode(node, renderer)) {
    for (auto iter = node->children().begin(); 
         iter != node->children().end(); ++iter) {
      RenderNodeRecusive(iter->get(), renderer);
    }
  }
}
}  // namespace lord
