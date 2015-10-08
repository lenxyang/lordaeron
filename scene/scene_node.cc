#include "lordaeron/scene/scene_node.h"

#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_util.h"
#include "base/strings/string_tokenizer.h"
#include "azer/render/render.h"
#include "azer/math/math.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
using namespace azer;
SceneNode::SceneNode() 
    : visible_(false),
      parent_(NULL),
      user_data_(NULL) {
}

SceneNode::SceneNode(const std::string& name)
    : visible_(false),
      parent_(NULL),
      name_(name),
      user_data_(NULL) {
}

SceneNode::~SceneNode() {
}

void SceneNode::AddChild(SceneNodePtr child) {
  DCHECK(child->parent() == NULL);
  child->parent_ = this;
  children_.push_back(child);
}

void SceneNode::RemoveChild(SceneNodePtr child) {
  DCHECK(child->parent_ == this);
  child->parent_ = NULL;

  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    if (iter->get() == child.get()) {
      children_.erase(iter);
      break;
    }
  }
}

bool SceneNode::HasAncestor(SceneNode* node) const {
  const SceneNode* cur = node->parent();
  while (cur) {
    if (cur == this) {
      return true;
    }

    cur = cur->parent();
  }

  return false;
}

SceneNode* SceneNode::root() {
  SceneNode* cur = this;
  while (cur) {
    if (cur->parent_ == NULL)
      return cur;
    cur = cur->parent();
  }

  return cur;
}

void SceneNode::set_name(const std::string& name) {
  name_ = name;
}

SceneNodePtr SceneNode::GetLocalChild(const std::string& name) {
  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    if ((*iter)->name() == name) {
      return *iter;
    }
  }
  return SceneNodePtr();
}

SceneNodePtr SceneNode::GetNode(const std::string& path) {
  if (StartsWithASCII(path, "//", false)) {
    return root()->GetNode(path.substr(2));
  } else {
    SceneNodePtr cur = this;
    ::base::StringTokenizer t(path, std::string("/"));
    while (t.GetNext()) {
      cur = cur->GetLocalChild(t.token());
      if (!cur.get())
        return SceneNodePtr();
    }
    return cur;
  }
}

std::string SceneNode::path() const {
  std::vector<std::string> path;
  const SceneNode* ptr = this;
  while (ptr) {
    path.push_back(ptr->name());
    ptr = ptr->parent();
  }

  std::string str = "/";
  for (auto iter = path.begin(); iter != path.end(); ++iter) {
    str.append("/");
    str.append(*iter);
  }
  return str;
}

void SceneNode::CreatePathRecusive(const std::string& path) {
}

bool SceneNode::AddChildAtPath(const std::string& parent, SceneNodePtr node) {
  SceneNodePtr pnode = GetNode(parent);
  if (pnode.get()) {
    pnode->AddChild(node);
    return true;
  } else {
    return false;
  }
}

SceneNodePtr SceneNode::RemoveChildAtPath(const std::string& path) {
  SceneNodePtr pnode = GetNode(path);
  if (pnode.get() && pnode->parent()) {
    pnode->parent()->RemoveChild(pnode.get());
  }
  return pnode;
}

SceneNode::Type SceneNode::type() const {
  if (data_.get()) {
    return data_->type();
  } else {
    return SceneNode::kEmptyNode;
  }
}

std::string SceneNode::print_info() {
  std::string str;
  print_info(&str, 0, this);
  return str;
}

void SceneNode::print_info(std::string* str, int depth, SceneNode* node) {
  Vector3 position = holder().position();
  str->append(std::string(depth, ' '));
  str->append("node[");
  str->append(::base::StringPrintf("name=%s,",name().c_str()));
  str->append(::base::StringPrintf(" pos={%f, %f, %f}", position.x,
                                   position.y, position.z));
  str->append("]\n");
  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    (*iter)->print_info(str, depth + 1, iter->get());
  }
}
}  // namespace lord
