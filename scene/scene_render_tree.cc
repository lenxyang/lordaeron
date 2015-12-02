#include "lordaeron/scene/scene_render_tree.h"

#include "base/logging.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
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

void SceneRenderEnvNode::UpdateParams(const azer::FrameArgs& args) {
  all_lights_.clear();
  if (parent) {
    all_lights_.assign(parent->all_lights_);
  }

  for (auto iter = lights_.begin(); iter != lights_.end(); ++iter) {
    all_lights_.push_back(*iter);
  }
}

// class SceneRenderNode
SceneRenderNode::SceneRenderNode(SceneNode* node, SceneRenderEnvNode* envnode)
    : node_(node),
      envnode_(envnode) {
  if (node->type() == SceneNode::kMeshNode) {
    AddMesh(node->mutable_data()->GetMesh());
  } else if (node->type() == SceneNode::kLampNode) {
    Light* light = node->mutable_data()->light();
    AddMesh(light->GetLightMesh(););
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

void SceneRenderNode::AddMesh(azer::Mesh* mesh) {
  mesh_.push_back(mesh);
}

void SceneRenderNode::Update(const azer::FrameArgs& args) {
  for (auto iter = mesh_.begin(); iter != mesh_.end(); ++iter) {
    (*iter)->UpdateProviderParams(args);
  }
}

void SceneRenderNode::Render(Renderer* renderer) {
  for (auto iter = mesh_.begin(); iter != mesh_.end(); ++iter) {
    (*iter)->Render(renderer);
  }
}

// class SceneRenderTreeBuilder
SceneRenderTreeBuilder::SceneRenderTreeBuilder() 
    : cur_(NULL) {
}

SceneRenderTreeBuilder::~SceneRenderTreeBuilder() {
}

void SceneRenderTreeBuilder::Bulid(SceneNode* root) {
  env_root_ = new SceneRenderEnvNode;
  root_ = new SceneRenderNode;
  root_->SetSceneRenderEnvNode(env_root_.get());
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
    continue;

  if (node->type() == kEnvNode) {
    CHECK(node->parent() != NULL);
    SceneRenderEnvNode* envnode = new SceneRenderEnvNode(cur_->GetRenderEnvNode());
    cur_->SetSceneRenderEnvNode(envnode);
  } else if (node->type() == kSceneNode) {
    SceneRenderNode* n = new SceneRenderNode(node);
    n->SetSceneRenderEnvNode(cur_->GetRenderEnvNode());
    cur_->AddChild(n);
    cur_ = n;
  } else if (node->type() == kMeshNode) {
    SceneRenderNode* n = new SceneRenderNode(node);
    n->SetSceneRenderEnvNode(cur_->GetRenderEnvNode());
    cur_->AddChild(n);
    cur_ = n;
  } else if (node->type() == kLampNode) {
    SceneRenderNode* n = new SceneRenderNode(node);
    n->SetSceneRenderEnvNode(cur_->GetRenderEnvNode());
    n->GetRenderEnvNode()->AddLight(node->data()->light());
    cur_->AddChild(n);
    cur_ = n;
  }
}

void SceneRenderTreeBuilder::OnTraverseNodeExit(SceneNode* node) {
  n->SetSceneRenderEnvNode(cur_->GetRenderEnvNode());
  cur_ = cur_->parent();
}

void SceneRenderTreeBuilder::OnTraverseEnd() {
  DCHECK(cur_ == root_.get());
}

}  // namespace lord
