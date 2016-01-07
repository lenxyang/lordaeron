#include "lordaeron/scene/scene_render_tree.h"

#include <sstream>
#include "base/logging.h"
#include "lordaeron/interactive/light_controller.h"
#include "lordaeron/scene/scene_env_tree.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_renderer.h"

namespace lord {
using namespace azer;
// class SceneRenderNodeDelegate
SceneRenderNodeDelegate::SceneRenderNodeDelegate(SceneRenderNode* node)
    : node_(node) {
}

SceneNode* SceneRenderNodeDelegate::GetSceneNode() {
  return node_->GetSceneNode();
}

const SceneNode* SceneRenderNodeDelegate::GetSceneNode() const {
  return node_->GetSceneNode();
}
SceneRenderNode* SceneRenderNodeDelegate::GetSceneRenderNode() { return node_;}
const SceneRenderNode* SceneRenderNodeDelegate::GetSceneRenderNode() const {
  return node_;
}

// class SceneRenderNode
SceneRenderNode::SceneRenderNode(SceneNode* node)
    : parent_(NULL),
      node_(node),
      envnode_(NULL),
      camera_(NULL) {
}

SceneRenderNode::~SceneRenderNode() {
}

void SceneRenderNode::SetDelegate(scoped_ptr<SceneRenderNodeDelegate> delegate) {
  delegate_ = delegate.Pass();
}

SceneRenderNode* SceneRenderNode::root() {
  SceneRenderNode* cur = this;
  while (cur->parent()) {
    cur = cur->parent();
  }
  return cur;
}

const Camera* SceneRenderNode::camera() const {
  return root()->camera_;
}

void SceneRenderNode::SetCamera(const azer::Camera* camera) {
  DCHECK(!camera_);
  DCHECK(!parent());
  camera_ = camera;
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
  DCHECK(!child->parent());
  DCHECK(!Contains(child));
  child->parent_ = this;
  children_.push_back(child);
}

void SceneRenderNode::SetEnvNode(SceneRenderEnvNode* node) {
  envnode_ = node;
  CHECK(children_.size() == 0u);
}

bool SceneRenderNode::RemoveChild(SceneRenderNode* child) {
  int32 index = GetIndexOf(child);
  if (index > 0) {
    children_.erase(children_.begin() + index);
    child->parent_ = NULL;
    return true;
  } else {
    return false;
  }
}

bool SceneRenderNode::Contains(SceneRenderNode* child) const {
  SceneRenderNode* cur = child;
  while (cur) {
    if (cur == this)
      return true;
    cur = cur->parent();
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
std::string SceneRenderNode::DumpTree() const {
  return std::move(DumpNode(this, 0));
}

std::string SceneRenderNode::DumpNode(const SceneRenderNode* node, int depth) const {
  SceneNode* snode = const_cast<SceneNode*>(node->GetSceneNode());
  std::stringstream ss;
  ss << std::string(depth, ' ') << "SceneRenderNode[" << (void*)node << "]"
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

void SceneRenderNode::UpdateParams(const azer::FrameArgs& args) {
}

bool SceneRenderNode::Init() {
  if (node_->type() == kLampSceneNode) {
    GetEnvNode()->AddLightNode(node_);
  }

  return true;
}

void SceneRenderNode::Update(const azer::FrameArgs& args) {
  CalcParams(args);
  if(delegate_.get())
    delegate_->Update(args);
}

void SceneRenderNode::Render(azer::Renderer* renderer) {
  if (delegate_.get())
    delegate_->Render(renderer);
}

void SceneRenderNode::CalcParams(const azer::FrameArgs& args) {
  // will be called on traverse the SceneRenderTree
  // if put the code into UpdateParams, the params will be calculated multily time
  // when SceneRenderNode used by multiple objects
  world_ = std::move(node_->holder().GenWorldMatrix());
  if (parent()) {
    world_ = std::move(parent()->world_ * world_);
  }
  pv_ = camera()->GetProjViewMatrix();
  pvw_ = std::move(pv_ * world_);
}

// class SceneRenderTreeBuilder
SceneRenderTreeBuilder::SceneRenderTreeBuilder(SceneNodeRenderDelegateFactory* fa) 
    : cur_(NULL),
      factory_(fa) {
}

SceneRenderTreeBuilder::~SceneRenderTreeBuilder() {
}

SceneRenderNodePtr SceneRenderTreeBuilder::Build(SceneNode* root, 
                                                 const Camera* camera) {
  DCHECK(cur_ == NULL);
  DCHECK(factory_ != NULL);
  SceneRenderNodePtr render_root = new SceneRenderNode(root);
  render_root->SetDelegate(factory_->CreateDelegate(render_root.get()).Pass());
  render_root->SetEnvNode(NULL);
  render_root->SetCamera(camera);
  cur_ = render_root.get();
  SceneNodeTraverse traverser(this);
  traverser.Traverse(root);
  cur_ = NULL;
  return render_root;
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
    SceneRenderEnvNodePtr envnode = new SceneRenderEnvNode(cur_->GetEnvNode());
    cur_->SetEnvNode(envnode);
  } else {
    SceneRenderNodePtr newnode = new SceneRenderNode(node);
    if (newnode) {
      newnode->SetEnvNode(cur_->GetEnvNode());
      newnode->SetDelegate(factory_->CreateDelegate(newnode).Pass());
      newnode->Init();
      cur_->AddChild(newnode);
      cur_ = newnode;
    }
  }
  return true;
}

void SceneRenderTreeBuilder::OnTraverseNodeExit(SceneNode* node) {
  if (cur_->GetSceneNode() == node) {
    cur_ = cur_->parent();
  }
}

void SceneRenderTreeBuilder::OnTraverseEnd() {
  DCHECK(cur_ == NULL);
}
}  // namespace lord
