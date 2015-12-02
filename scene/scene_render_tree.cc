#include "lordaeron/scene/scene_render_tree.h"

#include "base/logging.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using namespace azer;
// class SceneRenderEnvNode
SceneRenderEnvNode::SceneRenderEnvNode() 
    : parent_(NULL) {
}

SceneRenderEnvNode::SceneRenderEnvNode(SceneRenderEnvNode* parent) 
    : parent_(parent) {
}

SceneRenderEnvNode* SceneRenderEnvNode::root() {
  if (parent_ == NULL) {
    return this;
  } else {
    return parent()->root();
  }
}

SceneRenderEnvNode* SceneRenderEnvNode::parent() {
  return parent_;
}

int32 SceneRenderEnvNode::child_count() const {
  return static_cast<int32>(children_.size());
}

SceneRenderEnvNode* SceneRenderEnvNode::child_at(int32 index) {
  return children_.at(index).get();
}

void SceneRenderEnvNode::AddChild(SceneRenderEnvNode* child) {
  DCHECK(!Contains(child));
  children_.push_back(child);
}

bool SceneRenderEnvNode::Contains(SceneRenderEnvNode* child) const {
  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    if (iter->get() == child)
      return true;
  }

  return false;
}

int32 SceneRenderEnvNode::GetIndexOf(SceneRenderEnvNode* child) const {
  int32 index = 0;
  for (auto iter = children_.begin(); iter != children_.end(); ++iter, ++index) {
    if (iter->get() == child)
      return index;
  }

  return -1;
}

void SceneRenderEnvNode::UpdateParams(const FrameArgs& args) {
  all_lights_.clear();
  if (parent()) {
    all_lights_ = parent()->all_lights_;
  }

  for (auto iter = lights_.begin(); iter != lights_.end(); ++iter) {
    all_lights_.push_back(*iter);
  }
}

// class SceneRenderNode
SceneRenderNode::SceneRenderNode(SceneNode* node)
    : node_(node),
      envnode_(NULL) {
  if (node->type() == kMeshSceneNode) {
    AddMesh(node->mutable_data()->GetMesh());
  } else if (node->type() == kLampSceneNode) {
    Light* light = node->mutable_data()->light();
    AddMesh(light->GetLightMesh());
  }
}

SceneRenderNode::~SceneRenderNode() {
}

SceneRenderNode* SceneRenderNode::root() {
  if (parent_ == NULL) {
    return this;
  } else {
    return parent()->root();
  }
}

SceneRenderNode* SceneRenderNode::parent() {
  return parent_;
}

int32 SceneRenderNode::child_count() const {
  return static_cast<int32>(children_.size());
}

SceneRenderNode* SceneRenderNode::child_at(int32 index) {
  return children_.at(index).get();
}

void SceneRenderNode::AddChild(SceneRenderNode* child) {
  DCHECK(!Contains(child));
  children_.push_back(child);
}

bool SceneRenderNode::Contains(SceneRenderNode* child) const {
  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    if (iter->get() == child)
      return true;
  }

  return false;
}

int32 SceneRenderNode::GetIndexOf(SceneRenderNode* child) const {
  int32 index = 0;
  for (auto iter = children_.begin(); iter != children_.end(); ++iter, ++index) {
    if (iter->get() == child)
      return index;
  }

  return -1;
}

void SceneRenderNode::UpdateParams(const azer::FrameArgs& args) {
  world_ = node_->world();
}

void SceneRenderNode::AddMesh(Mesh* mesh) {
  /*
    light_mesh->AddProvider(new LightColorProvider(light_.get()));
    SceneNodeParamsPtr params(new SceneNodeParams(node_));
    light_mesh->AddProvider(params);
    light_mesh->AddProvider(node_->context()->GetGlobalEnvironment());
  */
  
  // mesh->AddProvider(this);
  // mesh->AddProvider(envnode_);
  mesh_ = mesh;
}

void SceneRenderNode::Update(const FrameArgs& args) {
  mesh_->UpdateProviderParams(args);
}

void SceneRenderNode::Render(Renderer* renderer) {
  mesh_->Render(renderer);
}

// class SceneRenderTreeBuilder
SceneRenderTreeBuilder::SceneRenderTreeBuilder() 
    : cur_(NULL) {
}

SceneRenderTreeBuilder::~SceneRenderTreeBuilder() {
}

void SceneRenderTreeBuilder::Bulid(SceneNode* root) {
  SceneRenderEnvNode* rootenv = new SceneRenderEnvNode;
  root_ = new SceneRenderNode(root);
  root_->SetSceneRenderEnvNode(rootenv);
  cur_ = root_.get();
  SceneNodeTraverse traverser(this);
  traverser.Traverse(root);
}

void SceneRenderTreeBuilder::OnTraverseBegin(SceneNode* root) {
}

bool SceneRenderTreeBuilder::OnTraverseNodeEnter(SceneNode* node) {
  if (!node->visible()) {
    return false;
  }

  if (!node->parent())
    return true;

  if (node->type() == kEnvSceneNode) {
    CHECK(node->parent() != NULL);
    SceneRenderEnvNode* envnode = new SceneRenderEnvNode(cur_->GetRenderEnvNode());
    cur_->SetSceneRenderEnvNode(envnode);
  } else if (node->type() == kSceneNode) {
    SceneRenderNode* n = new SceneRenderNode(node);
    n->SetSceneRenderEnvNode(cur_->GetRenderEnvNode());
    cur_->AddChild(n);
    cur_ = n;
  } else if (node->type() == kMeshSceneNode) {
    SceneRenderNode* n = new SceneRenderNode(node);
    n->SetSceneRenderEnvNode(cur_->GetRenderEnvNode());
    cur_->AddChild(n);
    cur_ = n;
  } else if (node->type() == kLampSceneNode) {
    SceneRenderNode* n = new SceneRenderNode(node);
    n->SetSceneRenderEnvNode(cur_->GetRenderEnvNode());
    n->GetRenderEnvNode()->AddLight(node->mutable_data()->light());
    cur_->AddChild(n);
    cur_ = n;
  }

  return true;
}

void SceneRenderTreeBuilder::OnTraverseNodeExit(SceneNode* node) {
  cur_ = cur_->parent();
}

void SceneRenderTreeBuilder::OnTraverseEnd() {
  DCHECK(cur_ == root_.get());
}


// class SimpleRenderTreeRenderer
SimpleRenderTreeRenderer::SimpleRenderTreeRenderer(SceneRenderNode* root)
    : root_(root) {
}

void SimpleRenderTreeRenderer::Update(const FrameArgs& args) {
  UpdateNode(root_, args);
}

void SimpleRenderTreeRenderer::Render(Renderer* renderer) {
  RenderNode(root_, renderer);
}

void SimpleRenderTreeRenderer::UpdateNode(SceneRenderNode* node, 
                                          const FrameArgs& args) {
  node->Update(args);
  for (auto iter = node->children().begin(); 
       iter != node->children().end(); ++iter) {
    UpdateNode(iter->get(), args);
  }
}

void SimpleRenderTreeRenderer::RenderNode(SceneRenderNode* node, 
                                          Renderer* renderer) {
  node->Render(renderer);
  for (auto iter = node->children().begin(); 
       iter != node->children().end(); ++iter) {
    RenderNode(iter->get(), renderer);
  }
}

}  // namespace lord
