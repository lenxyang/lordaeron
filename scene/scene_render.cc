#include "lordaeron/scene/scene_render.h"

#include "lordaeron/scene/scene_context.h"
#include "lordaeron/scene/scene_environment.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
SceneArgsUpdator::SceneArgsUpdator(SceneEnvironment* env)
    : environment_(env),
      args_(NULL) {
}

SceneArgsUpdator::~SceneArgsUpdator() {
}

void SceneArgsUpdator::OnTraverseBegin(SceneNode* root)  {
  environment_->reset();
  mesh_.clear();
}

bool SceneArgsUpdator::OnTraverseNodeEnter(SceneNode* node)  {
  UpdateNodeWorld(node);

  if (node->type() == SceneNode::kLampNode) {
    environment_->PushLight(node->mutable_data()->light());
  } else if (node->type() == SceneNode::kMeshNode) {
    azer::MeshPtr mesh = node->mutable_data()->GetMesh();
    mesh->UpdateParams(*args_);
    mesh_.push_back(mesh);
  } else {
  }
  return true;
}

void SceneArgsUpdator::OnTraverseNodeExit(SceneNode* node)  {
}

void SceneArgsUpdator::OnTraverseEnd()  {
}

void SceneArgsUpdator::UpdateNodeWorld(SceneNode* node) {
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
  delegate_.reset(new SceneArgsUpdator(context->GetNodeEnvironment()));
}

SceneRender::~SceneRender() {
}

void SceneRender::Update(const azer::FrameArgs& args) {
  delegate_->SetFrameArgs(&args);
  SceneNodeTraverse traverser(delegate_.get());
  traverser.Traverse(root_);
}

void SceneRender::Draw(azer::Renderer* renderer, azer::Effect* effect, 
                       azer::PrimitiveTopology primitive) {
  for (auto iter = delegate_->mesh().begin(); 
       iter != delegate_->mesh().end();  ++iter) {
    (*iter)->DrawIndex(renderer, effect, primitive);
  }
}
}  // namespace lord
