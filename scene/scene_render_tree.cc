#include "lordaeron/scene/scene_render_tree.h"

#include <sstream>
#include "base/logging.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/effect/light_mesh.h"

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
  SceneRenderEnvNode* cur = this;
  while (cur->parent()) {
    cur = cur->parent();
  }
  return cur;
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
  child->parent_ = this;
  children_.push_back(child);
}

bool SceneRenderEnvNode::RemoveChild(SceneRenderEnvNode* child) {
  int32 index = GetIndexOf(child);
  if (index > 0) {
    children_.erase(children_.begin() + index);
    child->parent_ = NULL;
    return true;
  } else {
    return false;
  }
}

bool SceneRenderEnvNode::Contains(SceneRenderEnvNode* child) const {
  SceneRenderEnvNode* cur = child;
  while (cur) {
    if (cur == this)
      return true;
    cur = cur->parent();
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

std::string SceneRenderEnvNode::DumpTree() const {
  return std::move(DumpNode(this, 0));
}

std::string SceneRenderEnvNode::DumpNode(const SceneRenderEnvNode* node,
                                         int depth) const {
  std::stringstream ss;
  ss << std::string(depth, ' ') << "SceneRenderEnvNode[" << (void*)node << "]"
     << std::endl;
  for (auto iter = node->children_.begin(); iter != node->children_.end(); ++iter) {
    ss << std::move(DumpNode(iter->get(), depth + 2));
  }
  return ss.str();
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
    : parent_(NULL),
      node_(node),
      envnode_(NULL),
      camera_(NULL) {
}

SceneRenderNode::~SceneRenderNode() {
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
  CHECK(mesh_.get() == NULL);
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
  std::stringstream ss;
  ss << std::string(depth, ' ') << "SceneRenderNode[" << (void*)node << "]"
     << " SceneNode=" << node->GetSceneNode()->name() << ", "
     << " type=" << SceneNodeName(node->GetSceneNode()->type()) << ", "
     << " envnode[" << (void*)node->envnode_.get() << "]"
     << std::endl;
  for (auto iter = node->children_.begin(); iter != node->children_.end(); ++iter) {
    ss << std::move(DumpNode(iter->get(), depth + 2));
  }
  return ss.str();
}

void SceneRenderNode::UpdateParams(const azer::FrameArgs& args) {
  // pvw_ computation must be here
  // otherwise, even no mesh need the provider, the pvw must be 
  // calculated.
  pvw_ = std::move(camera()->GetProjViewMatrix() * world_);
}

void SceneRenderNode::AddMesh(Mesh* mesh) {
  mesh_ = mesh;
}

void SceneRenderNode::Init() {
  Mesh* mesh = NULL;
  if (node_->type() == kMeshSceneNode) {
    mesh = node_->mutable_data()->GetMesh();
  } else if (node_->type() == kLampSceneNode) {
    Light* light = node_->mutable_data()->light();
    mesh = light->GetLightMesh();
    mesh->AddProvider(new LightColorProvider(light));
    GetEnvNode()->AddLight(light);
  } else if (node_->type() == kEnvSceneNode) {
  }

  if (mesh) {
    mesh->AddProvider(this);
    mesh->AddProvider(envnode_);
    AddMesh(mesh);
  }
}

void SceneRenderNode::Update(const FrameArgs& args) {
  if (parent()) {
    world_ = std::move(parent()->world_ * node_->holder().GenWorldMatrix());
  } else {
    world_ = Matrix4::kIdentity;
  }

  if (mesh_.get()) {
    mesh_->UpdateProviderParams(args);
  }
}

void SceneRenderNode::Render(Renderer* renderer) {
  if (mesh_.get())
    mesh_->Render(renderer);
}

// class SceneRenderTreeBuilder
SceneRenderTreeBuilder::SceneRenderTreeBuilder(SceneRenderNodeCreator* creator) 
    : cur_(NULL),
      creator_(creator) {
}

SceneRenderTreeBuilder::~SceneRenderTreeBuilder() {
}

void SceneRenderTreeBuilder::Build(SceneNode* root, const Camera* camera) {
  SceneRenderEnvNode* rootenv = new SceneRenderEnvNode;
  root_ = new SceneRenderNode(root);
  root_->SetCamera(camera);
  cur_ = root_.get();
  SceneNodeTraverse traverser(this);
  traverser.Traverse(root);
}

SceneRenderNodePtr SceneRenderTreeBuilder::GetRenderNodeRoot() {
  return root_;
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
    SceneRenderEnvNode* envnode = new SceneRenderEnvNode(cur_->GetEnvNode());
    cur_->SetEnvNode(envnode);
  }

  SceneRenderNode* newnode = creator_->Create(node);
  if (newnode) {
    newnode->SetEnvNode(cur_->GetEnvNode());
    newnode->Init();
    cur_->AddChild(newnode);
    cur_ = newnode;
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

SceneRenderNode* DefaultSceneRenderNodeCreator::Create(SceneNode* node) {
  switch (node->type()) {
    case kSceneNode:
    case kMeshSceneNode:
    case kLampSceneNode:
      return new SceneRenderNode(node);
    default:
      return NULL;
  }
}
}  // namespace lord
