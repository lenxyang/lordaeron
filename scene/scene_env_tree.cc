#include "lordaeron/scene/scene_env_tree.h"

#include <sstream>
#include "base/logging.h"
#include "lordaeron/interactive/light_controller.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_renderer.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {
using namespace azer;

namespace {
void UpdateSceneNodeLight(SceneNode* node) {
  if (node->type() == kLampSceneNode) {
    Light* light = node->mutable_data()->light();
    switch (light->type()) {
      case kDirectionalLight: 
        light->set_enable(node->visible());
        break;
      case kPointLight: 
        light->set_enable(node->visible());
        break;
      case kSpotLight: 
        light->set_enable(node->visible());
        break;
      default:
        CHECK(false);
    }
  }
}
}

// class SceneRenderEnvNode
SceneRenderEnvNode::SceneRenderEnvNode() : parent_(NULL) {}

SceneRenderEnvNode::SceneRenderEnvNode(SceneRenderEnvNode* parent) 
    : parent_(parent) {
}

SceneRenderEnvNode::~SceneRenderEnvNode() {}

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

void SceneRenderEnvNode::AddLightNode(SceneNode* node) {
  DCHECK(node->type() == kLampSceneNode);
  light_nodes_.push_back(node);
}

void SceneRenderEnvNode::UpdateParams(const FrameArgs& args) {
  all_lights_.clear();
  if (parent()) {
    all_lights_ = parent()->all_lights_;
  }

  for (auto iter = light_nodes_.begin(); iter != light_nodes_.end(); ++iter) {
    SceneNode* node = iter->get();
    DCHECK(node->type() == kLampSceneNode);
    UpdateSceneNodeLight(node);
    all_lights_.push_back(node->mutable_data()->light());
  }
}
}  // namespace lord
