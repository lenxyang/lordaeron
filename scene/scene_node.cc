#include "lordaeron/scene/scene_node.h"

#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_util.h"
#include "base/strings/string_tokenizer.h"
#include "azer/render/render.h"
#include "azer/math/math.h"
#include "lordaeron/scene/scene_bounding_volumn.h"
#include "lordaeron/scene/scene_context.h"
#include "lordaeron/scene/scene_node_data.h"
#include "lordaeron/render/bounding_volumn.h"
#include "lordaeron/render/axis_aligned_bounding_box.h"

namespace lord {
using namespace azer;
SceneNode::SceneNode() 
    : visible_(true),
      pickable_(false),
      shadow_caster_(false),
      parent_(NULL),
      user_data_(NULL) {
  data_.reset(new SceneNodeData(this));
}

SceneNode::SceneNode(SceneContextPtr context)
    : visible_(true),
      pickable_(false),
      shadow_caster_(false),
      parent_(NULL),
      user_data_(NULL),
      context_(context) {
  data_.reset(new SceneNodeData(this));
}

SceneNode::SceneNode(const std::string& name)
    : visible_(true),
      pickable_(false),
      shadow_caster_(false),
      parent_(NULL),
      name_(name),
      user_data_(NULL) {
  data_.reset(new SceneNodeData(this));
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

void SceneNode::set_draw_bounding_volumn(bool b) {
  if (b) {
    if (!bounding_volumn_.get()) {
      bounding_volumn_ = CreateBoundingBoxForSceneNode(this);
    }
  } else {
    bounding_volumn_ = NULL;
  }
}

bool SceneNode::is_draw_bounding_volumn() const {
  return bounding_volumn_.get() != NULL;
}

azer::Mesh* SceneNode::bounding_volumn() {
  return bounding_volumn_.get();
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

SceneContext* SceneNode::context() {
  if (parent()) {
    return parent()->context();
  } else {
    return context_.get();
  }
}

void SceneNode::CalcChildrenBoundingVector() {
  azer::Vector3 vmin = azer::Vector3(99999.0f, 99999.0f, 99999.0f);
  azer::Vector3 vmax = azer::Vector3(-99999.0f, -99999.0f, -99999.0f);
  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    SceneNode* child = (*iter).get();
    UpdateVMinAndVMax(child->vmin(), &vmin, &vmax);
    UpdateVMinAndVMax(child->vmax(), &vmin, &vmax);
  }

  SetMin(vmin);
  SetMax(vmax);
}

void SceneNode::UpdateBoundingHierarchy() {
  SceneNode* cur = this;
  while (cur) {
    cur->CalcChildrenBoundingVector();
    cur = cur->parent();
  }
}

void SceneNode::BoundingChanged(const azer::Vector3& orgmin, 
                                const azer::Vector3& orgmax) {
  if (parent()) {
    parent()->CalcChildrenBoundingVector();
  }
}

azer::Vector3 SceneNode::RevertTranformOnPos(const azer::Vector3& v) {
  azer::Vector3 inverse_scale = scale().InverseCopy();
  azer::Vector3 ret = v;
  ret -= holder().position();
  ret.x = inverse_scale.x * ret.x;
  ret.y = inverse_scale.y * ret.y;
  ret.z = inverse_scale.z * ret.z;
  return ret;
}

azer::Vector3  SceneNode::ApplyTranformOnPos(const azer::Vector3 &v) {
  azer::Vector3 s = scale();
  azer::Vector3 ret = v;
  ret.x = s.x * ret.x;
  ret.y = s.y * ret.y;
  ret.z = s.z * ret.z;

  ret += holder().position();
  return ret;
}

void SceneNode::SetPosition(const azer::Vector3& pos) {
  azer::Vector3 org_vmin = vmin_;
  azer::Vector3 org_vmax = vmax_;
  vmin_ = RevertTranformOnPos(vmin_);
  vmax_ = RevertTranformOnPos(vmin_);
  mutable_holder()->SetPosition(pos);
  vmin_ = ApplyTranformOnPos(vmin_);
  vmax_ = ApplyTranformOnPos(vmax_);

  if (vmin_ != org_vmin || vmax_ != org_vmax) {
    BoundingChanged(org_vmin, org_vmax);
  }
}

void SceneNode::SetScale(const azer::Vector3& v) {
  using namespace azer;
  azer::Vector3 org_vmin = vmin_;
  azer::Vector3 org_vmax = vmax_;
  vmin_ = RevertTranformOnPos(vmin_);
  vmax_ = RevertTranformOnPos(vmin_);
  mutable_holder()->SetScale(v);
  vmin_ = ApplyTranformOnPos(vmin_);
  vmax_ = ApplyTranformOnPos(vmax_);
  if (vmin_ != org_vmin || vmax_ != org_vmax) {
    BoundingChanged(org_vmin, org_vmax);
  }
}

void SceneNode::SetMin(const azer::Vector3& v) {
  azer::Vector3 s = holder().scale();
  azer::Vector3 pos = holder().position();

  azer::Vector3 org_vmin = vmin_;
  vmin_ = ApplyTranformOnPos(v);

  if (vmin_ != org_vmin) {
    BoundingChanged(org_vmin, vmax_);
  }
}

void SceneNode::SetMax(const azer::Vector3& v) {
  azer::Vector3 s = holder().scale();
  azer::Vector3 pos = holder().position();
  azer::Vector3 org_vmax = vmax_;
  vmax_ = ApplyTranformOnPos(v);
  if (vmax_ != org_vmax) {
    BoundingChanged(vmin_, org_vmax);
  }
}

void SceneNode::set_orientation(const azer::Quaternion& q) {
  mutable_holder()->set_orientation(q);
}
}  // namespace lord
