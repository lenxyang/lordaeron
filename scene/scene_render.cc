#include "lordaeron/scene/scene_render.h"

#include "lordaeron/scene/scene_environment.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
NodeRendererTraverseDelegate::NodeRendererTraverseDelegate(SceneEnvironment* env)
    : environment_(env) {
}

NodeRendererTraverseDelegate::~NodeRendererTraverseDelegate() {
}

void NodeRendererTraverseDelegate::OnTraverseBegin(SceneNode* root)  {
  environment_->reset();
}

bool NodeRendererTraverseDelegate::OnTraverseNodeEnter(SceneNode* node)  {
  UpdateNodeWorld(node);

  if (node->type() == SceneNode::kLampNode) {
    environment_->PushLight(node->data()->light());
  } else if (node->type() == SceneNode::kMeshNode) {
  } else {
  }
  return true;
}

void NodeRendererTraverseDelegate::OnTraverseNodeExit(SceneNode* node)  {
}

void NodeRendererTraverseDelegate::OnTraverseEnd()  {
}

void NodeRendererTraverseDelegate::UpdateNodeWorld(SceneNode* node) {
  azer::Matrix4 parent_world;
  if (node->parent()) {
    parent_world = std::move(node->parent()->world());
  } else {
    parent_world = azer::Matrix4::kIdentity;
  }
  
  azer::Matrix4 local = node->holder().GenWorldMatrix();
  *node->mutable_world() = std::move(parent_world * local);
}

SceneRender::SceneRender(SceneContext* context, SceneNode* root) 
    : context_(context),
      root_(root) {
}

SceneRender::~SceneRender() {
}

void SceneRender::Update(const azer::FrameArgs& args) {
}

void SceneRender::Draw(azer::Renderer* renderer) {
}
}  // namespace lord
