#include "lordaeron/scene/render_node.h"

#include <sstream>
#include "base/logging.h"
#include "lordaeron/interactive/light_controller.h"
#include "lordaeron/scene/render_env_node.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_renderer.h"

namespace lord {
using namespace azer;
// class RenderNodeDelegate
RenderNodeDelegate::RenderNodeDelegate(RenderNode* node)
    : node_(node) {
}

SceneNode* RenderNodeDelegate::GetSceneNode() {
  return node_->GetSceneNode();
}

const SceneNode* RenderNodeDelegate::GetSceneNode() const {
  return node_->GetSceneNode();
}
RenderNode* RenderNodeDelegate::GetRenderNode() { return node_;}
const RenderNode* RenderNodeDelegate::GetRenderNode() const {
  return node_;
}

// class RenderNode
RenderNode::RenderNode(SceneNode* node)
    : parent_(NULL),
      node_(node),
      envnode_(NULL),
      camera_(NULL) {
}

RenderNode::~RenderNode() {
}

void RenderNode::SetDelegate(scoped_ptr<RenderNodeDelegate> delegate) {
  delegate_ = delegate.Pass();
}

RenderNode* RenderNode::root() {
  RenderNode* cur = this;
  while (cur->parent()) {
    cur = cur->parent();
  }
  return cur;
}

const Camera* RenderNode::camera() const {
  return root()->camera_;
}

void RenderNode::SetCamera(const azer::Camera* camera) {
  DCHECK(!camera_);
  DCHECK(!parent());
  camera_ = camera;
}

RenderNode* RenderNode::parent() {
  return parent_;
}

int32 RenderNode::child_count() const {
  return static_cast<int32>(children_.size());
}

RenderNode* RenderNode::child_at(int32 index) {
  return children_.at(index).get();
}

void RenderNode::AddChild(RenderNode* child) {
  DCHECK(!child->parent());
  DCHECK(!Contains(child));
  child->parent_ = this;
  children_.push_back(child);
}

void RenderNode::SetEnvNode(RenderEnvNode* node) {
  envnode_ = node;
  CHECK(children_.size() == 0u);
}

bool RenderNode::RemoveChild(RenderNode* child) {
  int32 index = GetIndexOf(child);
  if (index > 0) {
    children_.erase(children_.begin() + index);
    child->parent_ = NULL;
    return true;
  } else {
    return false;
  }
}

bool RenderNode::Contains(RenderNode* child) const {
  RenderNode* cur = child;
  while (cur) {
    if (cur == this)
      return true;
    cur = cur->parent();
  }
  return false;
}

int32 RenderNode::GetIndexOf(RenderNode* child) const {
  int32 index = 0;
  for (auto iter = children_.begin(); iter != children_.end(); ++iter, ++index) {
    if (iter->get() == child)
      return index;
  }

  return -1;
}
std::string RenderNode::DumpTree() const {
  return std::move(DumpNode(this, 0));
}

std::string RenderNode::DumpNode(const RenderNode* node, int depth) const {
  SceneNode* snode = const_cast<SceneNode*>(node->GetSceneNode());
  std::stringstream ss;
  ss << std::string(depth, ' ') << "RenderNode[" << (void*)node << "]"
     << " SceneNode=" << snode->name() << ", "
     << " type=" << SceneNodeName(snode->type()) << ", "
     << " envnode[" << (void*)node->envnode_.get() << "]";
  if (snode->type() == kObjectSceneNode) {
    Mesh* mesh = snode->mutable_data()->GetMesh();
    Effect* effect = mesh->part_at(0)->effect();
    ss << " Object Effect: " << effect->GetEffectName();
  }
  ss << std::endl;

  for (auto iter = node->children_.begin(); iter != node->children_.end(); ++iter) {
    ss << std::move(DumpNode(iter->get(), depth + 2));
  }
  return ss.str();
}

void RenderNode::UpdateParams(const azer::FrameArgs& args) {
}

bool RenderNode::Init() {
  return true;
}

void RenderNode::Update(const azer::FrameArgs& args) {
  CalcParams(args);
  if(delegate_.get())
    delegate_->Update(args);
}

void RenderNode::Render(azer::Renderer* renderer) {
  if (delegate_.get())
    delegate_->Render(renderer);
}

void RenderNode::CalcParams(const azer::FrameArgs& args) {
  // will be called on traverse the RenderTree
  // if put the code into UpdateParams, the params will be calculated multily time
  // when RenderNode used by multiple objects
  world_ = std::move(node_->holder().GenWorldMatrix());
  if (parent()) {
    world_ = std::move(parent()->world_ * world_);
  }
  pv_ = camera()->GetProjViewMatrix();
  pvw_ = std::move(pv_ * world_);
}

// class RenderTreeBuilderDelegate
bool RenderTreeBuilderDelegate::NeedRenderEnvNode(SceneNode* node) {
  if (node->type() != kEnvSceneNode) {
    return false;
  } else {
    return true;
  }
}

// class RenderTreeBuilder
RenderTreeBuilder::RenderTreeBuilder(RenderTreeBuilderDelegate* delegate) 
    : cur_(NULL),
      delegate_(delegate) {
}

RenderTreeBuilder::~RenderTreeBuilder() {
}

RenderNodePtr RenderTreeBuilder::Build(SceneNode* root, const Camera* camera) {
  DCHECK(cur_ == NULL);
  DCHECK(delegate_ != NULL);
  RenderNodePtr render_root = new RenderNode(root);
  render_root->SetDelegate(delegate_->CreateRenderDelegate(
      render_root.get()).Pass());
  RenderEnvNodePtr envnode = new RenderEnvNode(NULL);
  envnode->set_delegate(delegate_->CreateEnvDelegate(envnode));
  render_root->SetEnvNode(envnode);
  render_root->SetCamera(camera);
  cur_ = render_root.get();
  SceneNodeTraverse traverser(this);
  traverser.Traverse(root);
  cur_ = NULL;
  return render_root;
}

void RenderTreeBuilder::OnTraverseBegin(SceneNode* root) {
}

bool RenderTreeBuilder::OnTraverseNodeEnter(SceneNode* scene_node) {
  if (!scene_node->parent())
    return true;

  RenderNodePtr render_node = new RenderNode(scene_node);
  RenderEnvNode* envnode = cur_->GetEnvNode();
  if (scene_node->type() == kEnvSceneNode) {
    CHECK(scene_node->parent() != NULL);
    RenderEnvNode* parent = cur_->GetEnvNode();
    envnode = new RenderEnvNode(NULL);
    parent->AddChild(envnode);
    RenderEnvNodeDelegatePtr delegate = delegate_->CreateEnvDelegate(envnode);
    delegate->Init(scene_node, render_node);
    envnode->set_delegate(delegate);
    cur_->SetEnvNode(envnode);
  }

  render_node->SetEnvNode(envnode);
  render_node->SetDelegate(delegate_->CreateRenderDelegate(render_node).Pass());
  render_node->Init();
  if (delegate_->NeedRenderNode(scene_node)) {
    cur_->AddChild(render_node);
    cur_ = render_node;
    return true;
  } else {
    return false;
  }
}

void RenderTreeBuilder::OnTraverseNodeExit(SceneNode* node) {
  if (cur_->GetSceneNode() == node) {
    cur_ = cur_->parent();
  }
}

void RenderTreeBuilder::OnTraverseEnd() {
  DCHECK(cur_ == NULL);
}
}  // namespace lord
