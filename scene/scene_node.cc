#include "lordaeron/scene/scene_node.h"

#include <atomic>
#include "base/logging.h"
#include "base/lazy_instance.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_util.h"
#include "base/strings/string_tokenizer.h"
#include "azer/render/render.h"
#include "azer/math/math.h"
#include "lordaeron/interactive/light_controller.h"
#include "lordaeron/scene/scene_node_observer.h"

namespace lord {
using namespace azer;

namespace {
void CalcSceneOrientForZDirection(const Vector3& d, Quaternion* orient) {
  Camera camera;
  Vector3 dir = d.NormalizeCopy();
  Vector3 up = Vector3(dir.x, 10.0f, dir.z);
  Vector3 newz = dir.NormalizeCopy();
  Vector3 newx = std::move(up.cross(newz).NormalizeCopy());
  Vector3 newy = std::move(newz.cross(newx).NormalizeCopy());
  *orient = Quaternion::FromAxis(newx, newy, newz);
  Vector3 newzz = orient->zaxis();
}

class SceneIdAllocator {
 public:
  SceneIdAllocator() : allocated_(0) {}
  int32 allocate() { return ++allocated_;}
 private:
  std::atomic<int32> allocated_;
  DISALLOW_COPY_AND_ASSIGN(SceneIdAllocator);
};
static ::base::LazyInstance<SceneIdAllocator> id_alloc = LAZY_INSTANCE_INITIALIZER;
}

SceneNodeManager::SceneNodeManager() {}

bool SceneNodeManager::Register(SceneNode* node) {
  if (dict_.end() == dict_.find(node->id())) {
    dict_.insert(std::make_pair(node->id(), node));
    return true;
  } else {
    return false;
  }
}

bool SceneNodeManager::Unregister(SceneNode* node) {
  auto iter = dict_.find(node->id());
  if (dict_.end() != iter) {
    dict_.erase(iter);
    return true;
  } else {
    return false;
  }
}

SceneNode* SceneNodeManager::Lookup(int32 id) {
  auto iter = dict_.find(id);
  if (dict_.end() != iter) {
    return iter->second;
  } else {
    return NULL;
  }
}
static ::base::LazyInstance<SceneNodeManager> scene_node_mgr =
    LAZY_INSTANCE_INITIALIZER;

// class SceneNodeData
SceneNodeData::SceneNodeData(SceneNode* node)
    : node_(node) {
  node->AddObserver(this);
}

SceneNodeData::~SceneNodeData() {
  // neednot to Remove observer, because it will be release by SceneNode
  // node_->RemoveObserver(this);
}

Mesh* SceneNodeData::GetMesh() {
  DCHECK(mesh_.get());
  return mesh_.get();
}

void SceneNodeData::reset() {
  if (light_.get()) {
    light_ = NULL;
  }

  mesh_ = NULL;
  Vector3 vmin(0.0f, 0.0f, 0.0f);
  Vector3 vmax(0.01f, 0.01f, 0.01f);
  node_->SetNodeType(kSceneNode);
  node_->SetLocalBounds(vmin, vmax);
}

Light* SceneNodeData::light() { 
  return light_.get();
}

void SceneNodeData::AttachMesh(MeshPtr mesh) {
  mesh_ = mesh;
  node_->SetNodeType(kObjectSceneNode);
  node_->SetLocalBounds(mesh->vmin(), mesh->vmax());
}

void SceneNodeData::AttachLight(Light* light) {
  DCHECK(light);
  light_ = light;
  node_->SetNodeType(kLampSceneNode);

  if (light->type() == kDirectionalLight || light->type() == kSpotLight) {
    Quaternion orient;
    CalcSceneOrientForZDirection(light->directional(), &orient);
    node_->set_orientation(orient);
  }

  if (light->type() == kPointLight || light->type() == kSpotLight) {
    light->set_position(node_->position());
  }
}

void SceneNodeData::OnNodeLocationChanged(SceneNode* node, const Vector3& prevpos) {
  if (node_->type() == kLampSceneNode) {
    if (light_->type() == kPointLight
        || light_->type() == kSpotLight) {
      light_->set_position(node->GetWorldPosition());
    }
  }
}

void SceneNodeData::OnNodeOrientChanged(SceneNode* node, const Quaternion& prev) {
  if (node_->type() == kLampSceneNode) {
    Matrix4 rotation = std::move(node->orientation().ToMatrix());
    Vector4 dir = rotation * Vector4(0.0f, 0.0f, 1.0f, 0.0f);
    if (light_->type() == kDirectionalLight
        || light_->type() == kSpotLight) {
      light_->set_directional(Vector3(dir.x, dir.y, dir.z));
    }
  }
}

// class SceneNode
SceneNode* SceneNode::Lookup(int32 id) {
  return scene_node_mgr.Pointer()->Lookup(id);
}

SceneNode::SceneNode() {
  InitMember();
}

SceneNode::SceneNode(const std::string& name) {
  InitMember();
  name_ = name;
}

SceneNode::SceneNode(const std::string& name, SceneNode* parent) {
  DCHECK(parent != this);
  InitMember();
  name_ = name;
  parent->AddChild(this);
}

SceneNode::SceneNode(const std::string& name, SceneNodeType type,
                     SceneNode* parent) {
  InitMember();
  name_ = name;
  type_ = type;
  parent->AddChild(this);
}

void SceneNode::InitMember() {
  id_ = id_alloc.Pointer()->allocate();
  scene_node_mgr.Pointer()->Register(this);
  visible_ = true;
  pickable_ = false;
  picked_ = false;
  shadow_caster_ = true;
  parent_ = NULL;
  type_ = kSceneNode;
  user_data_ = NULL;
  data_.reset(new SceneNodeData(this));
  draw_bounding_ = false;
}

SceneNode::~SceneNode() {
  scene_node_mgr.Pointer()->Unregister(this);
}

void SceneNode::AddChild(SceneNode* child) {
  DCHECK(child != this);
  DCHECK(child->parent() == NULL);
  child->parent_ = this;
  children_.push_back(child);
}

void SceneNode::RemoveChild(SceneNode* child) {
  DCHECK(child->parent_ == this);
  child->parent_ = NULL;

  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    if (iter->get() == child) {
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

Vector3 SceneNode::GetWorldPosition() const {
  Vector3 pos;
  const SceneNode* cur = this;
  while (cur) {
    pos += cur->position();
    cur = cur->parent();
  }
  return pos;
}

SceneNode* SceneNode::GetLocalChild(const std::string& name) {
  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    if ((*iter)->name() == name) {
      return *iter;
    }
  }
  return NULL;
}

SceneNode* SceneNode::GetNode(const std::string& path) {
  if (StartsWithASCII(path, "//", false)) {
    return root()->GetNode(path.substr(2));
  } else {
    SceneNodePtr cur = this;
    ::base::StringTokenizer t(path, std::string("/"));
    while (t.GetNext()) {
      cur = cur->GetLocalChild(t.token());
      if (!cur.get())
        return NULL;
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

bool SceneNode::AddChildAtPath(const std::string& parent, SceneNode* node) {
  SceneNode* pnode = GetNode(parent);
  if (pnode) {
    pnode->AddChild(node);
    return true;
  } else {
    return false;
  }
}

SceneNode* SceneNode::RemoveChildAtPath(const std::string& path) {
  SceneNode* pnode = GetNode(path);
  if (pnode && pnode->parent()) {
    pnode->parent()->RemoveChild(pnode);
  }
  return pnode;
}

SceneNodeType SceneNode::type() const {
  return type_;
}

void SceneNode::set_draw_bounding_volumn(bool b) {
  draw_bounding_ = b;
}

bool SceneNode::is_draw_bounding_volumn() const {
  return draw_bounding_;
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

void SceneNode::UpdateBoundsByChildren() {
  Vector3 vmin = Vector3(99999.0f, 99999.0f, 99999.0f);
  Vector3 vmax = Vector3(-99999.0f, -99999.0f, -99999.0f);
  for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
    SceneNode* child = (*iter).get();
    Matrix4 mat = std::move(child->holder().GenWorldMatrix());
    Vector3 child_vmin = mat * Vector4(child->local_vmin(), 1.0);
    Vector3 child_vmax = mat * Vector4(child->local_vmax(), 1.0);
    UpdateVMinAndVMax(child_vmin, &vmin, &vmax);
    UpdateVMinAndVMax(child_vmax, &vmin, &vmax);
  }

  SetLocalBounds(vmin, vmax);
}

void SceneNode::SetPosition(const Vector3& pos) {
  Vector3 oldpos = this->position();
  mutable_holder()->SetPosition(pos);
  LocationChanged(oldpos);
}

void SceneNode::SetScale(const Vector3& v) {
  using namespace azer;
  Vector3 org_scale = holder().scale();
  mutable_holder()->SetScale(v);
  if (v != org_scale) {
    ScaleChanged(org_scale);
  }
}

void SceneNode::SetLocalBounds(const Vector3& vmin, const Vector3& vmax) {
  local_vmin_ = vmin;
  local_vmax_ = vmax;
  NotifyParentBoundsChanged();
}

void SceneNode::pitch(const Radians angle) {
  Quaternion oldq = holder().orientation();
  mutable_holder()->pitch(angle);
  OrientationChanged(oldq);
}

void SceneNode::yaw(const Radians angle) {
  Quaternion oldq = holder().orientation();
  mutable_holder()->yaw(angle);
  OrientationChanged(oldq);
}

void SceneNode::roll(const Radians angle) {
  Quaternion oldq = holder().orientation();
  mutable_holder()->roll(angle);
  OrientationChanged(oldq);
}

void SceneNode::pitch(const Degree angle) {
  Quaternion oldq = holder().orientation();
  mutable_holder()->pitch(angle);
  OrientationChanged(oldq);
}

void SceneNode::yaw(const Degree angle) {
  Quaternion oldq = holder().orientation();
  mutable_holder()->yaw(angle);
  OrientationChanged(oldq);
}

void SceneNode::roll(const Degree angle) {
  Quaternion oldq = holder().orientation();
  mutable_holder()->roll(angle);
  OrientationChanged(oldq);
}

void SceneNode::rotate(const Vector3& axis, const Degree angle) {
  Quaternion oldq = holder().orientation();
  mutable_holder()->rotate(axis, angle);
  OrientationChanged(oldq);
}
void SceneNode::rotate(const Vector3& axis, const Radians angle) {
  Quaternion oldq = holder().orientation();
  mutable_holder()->rotate(axis, angle);
  OrientationChanged(oldq);
}

void SceneNode::set_orientation(const Quaternion& q) {
  Quaternion oldq = holder().orientation();
  mutable_holder()->set_orientation(q);
  OrientationChanged(oldq);
}

// observers
void SceneNode::LocationChanged(const Vector3& orgpos) {
  FOR_EACH_OBSERVER(SceneNodeObserver, 
                    observers_, 
                    OnNodeLocationChanged(this, orgpos));
  NotifyParentBoundsChanged();
}

void SceneNode::ScaleChanged(const Vector3& org_scale) {
  FOR_EACH_OBSERVER(SceneNodeObserver, 
                    observers_, 
                    OnNodeScaleChanged(this, org_scale));
  NotifyParentBoundsChanged();
}

void SceneNode::OrientationChanged(const Quaternion& origin_orient) {
  FOR_EACH_OBSERVER(SceneNodeObserver, 
                    observers_, 
                    OnNodeOrientChanged(this, origin_orient));
  NotifyParentBoundsChanged();
}

void SceneNode::NotifyParentBoundsChanged() {
  if (parent()) {
    // it will recusive call its' parent, if its local bounds changed.
    parent()->UpdateBoundsByChildren();
  }
}

void SceneNode::SetNodeType(SceneNodeType type) {
  DCHECK(type_ == kSceneNode);
  type_ = type;
}

void SceneNode::AddObserver(SceneNodeObserver* observer) {
  observers_.AddObserver(observer);
}

void SceneNode::RemoveObserver(SceneNodeObserver* observer) {
  observers_.RemoveObserver(observer);
}

bool SceneNode::HasObserver(SceneNodeObserver* observer) {
  return observers_.HasObserver(observer);
}

void SceneNode::AddAttr(const std::string& name, const std::string& value) {
  attributes_.insert(std::make_pair(name, value));
}
std::string SceneNode::GetAttr(const std::string& name) const {
  auto iter = attributes_.find(name);
  if (iter != attributes_.end()) {
    return iter->second;
  } else {
    return std::string("");
  }
}

const char* SceneNodeName(int32 type) {
  switch (type) {
    case kSceneNode: return "SceneNode";
    case kEnvSceneNode: return "EnvNode";
    case kObjectSceneNode: return "ObjectNode";
    case kLampSceneNode: return "LampNode";
    case kCameraSceneNode: return "CameraNode";
    case kTerrainTileSceneNode: return "TerrainNode";
    default: CHECK(false);return "";
  }
}

Matrix4 GenWorldMatrixForSceneNode(SceneNode* node) {
  Matrix4 world = Matrix4::kIdentity;
  SceneNode* cur = node;
  while (cur) {
    world = std::move(cur->holder().GenWorldMatrix() * world);
    cur = cur->parent();
  }
  return world;
}

void GetSceneNodeBounds(SceneNode* node, Vector3* vmin, Vector3* vmax) {
  Matrix4 mat = std::move(GenWorldMatrixForSceneNode(node));
  *vmin = mat * Vector4(node->local_vmin(), 1.0);
  *vmax = mat * Vector4(node->local_vmax(), 1.0);
}
}  // namespace lord
