#include "lordaeron/scene/scene_node.h"

#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_util.h"
#include "base/strings/string_tokenizer.h"
#include "azer/render/render.h"
#include "azer/math/math.h"
#include "lordaeron/interactive/light_mesh.h"
#include "lordaeron/scene/scene_bounding_volumn.h"
#include "lordaeron/scene/scene_node_observer.h"

namespace lord {
using namespace azer;

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
  node_->SetNodeType(kSceneNode);
  node_->SetMin(Vector3(0.0f, 0.0f, 0.0f));
  node_->SetMax(Vector3(0.0f, 0.0f, 0.0f));
}

Light* SceneNodeData::light() { 
  return light_.get();
}

void SceneNodeData::AttachMesh(MeshPtr mesh) {
  mesh_ = mesh;
  node_->SetNodeType(kMeshSceneNode);
  node_->SetMin(mesh->vmin());
  node_->SetMax(mesh->vmax());
}

void SceneNodeData::AttachLight(Light* light) {
  DCHECK(light);
  light_ = light;
  node_->SetNodeType(kLampSceneNode);

  // just calculate the min and max
  MeshPtr light_mesh = CreateLightMesh(node_);
  DCHECK(light_mesh.get());
  node_->SetMin(light_mesh->vmin());
  node_->SetMax(light_mesh->vmax());
}

void SceneNodeData::OnSceneNodeLocationChanged(SceneNode* node, 
                                               const Vector3& prevpos) {
  if (node_->type() == kLampSceneNode) {
    if (light_->type() == kPointLight) {
      light_->mutable_point_light()->position = node->GetWorldPosition();
    } else if (light_->type() == kSpotLight) {
      light_->mutable_spot_light()->position = node->GetWorldPosition();
    }
  }
}

void SceneNodeData::OnSceneNodeOrientationChanged(
    SceneNode* node, const azer::Quaternion& prev_orient) {
  if (node_->type() == kLampSceneNode) {
    Matrix4 rotation = std::move(node->orientation().ToMatrix());
    Vector4 dir = rotation * Vector4(0.0f, 0.0f, 1.0f, 0.0f);
    if (light_->type() == kDirectionalLight) {
      light_->mutable_dir_light()->direction = Vector3(dir.x, dir.y, dir.z);
    } else if (light_->type() == kSpotLight) {
      light_->mutable_spot_light()->direction = Vector3(dir.x, dir.y, dir.z);
    }
  }
}

// class SceneNode
SceneNode::SceneNode() {
  InitMember();
}

SceneNode::SceneNode(const std::string& name) {
  InitMember();
  name_ = name;
}

SceneNode::SceneNode(const std::string& name, SceneNode* parent) {
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
  visible_ = true;
  pickable_ = false;
  picked_ = false;
  shadow_caster_ = false;
  parent_ = NULL;
  type_ = kSceneNode;
  user_data_ = NULL;
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

Vector3 SceneNode::GetWorldPosition() const {
  Vector3 pos;
  const SceneNode* cur = this;
  while (cur) {
    pos += cur->position();
    cur = cur->parent();
  }
  return pos;
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

SceneNodeType SceneNode::type() const {
  return type_;
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

Mesh* SceneNode::bounding_volumn() {
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

void SceneNode::CalcChildrenBoundingVector() {
  Vector3 vmin = Vector3(99999.0f, 99999.0f, 99999.0f);
  Vector3 vmax = Vector3(-99999.0f, -99999.0f, -99999.0f);
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

Vector3 SceneNode::RevertTranformOnPos(const Vector3& v) {
  Vector3 inverse_scale = scale().InverseCopy();
  Vector3 ret = v;
  ret -= holder().position();
  ret.x = inverse_scale.x * ret.x;
  ret.y = inverse_scale.y * ret.y;
  ret.z = inverse_scale.z * ret.z;
  return ret;
}

Vector3  SceneNode::ApplyTranformOnPos(const Vector3 &v) {
  Vector3 s = scale();
  Vector3 ret = v;
  ret.x = s.x * ret.x;
  ret.y = s.y * ret.y;
  ret.z = s.z * ret.z;

  ret += holder().position();
  return ret;
}

void SceneNode::SetPosition(const Vector3& pos) {
  Vector3 oldpos = this->position();
  Vector3 org_vmin = vmin_;
  Vector3 org_vmax = vmax_;
  vmin_ = RevertTranformOnPos(vmin_);
  vmax_ = RevertTranformOnPos(vmax_);
  mutable_holder()->SetPosition(pos);
  vmin_ = ApplyTranformOnPos(vmin_);
  vmax_ = ApplyTranformOnPos(vmax_);

  if (vmin_ != org_vmin || vmax_ != org_vmax) {
    BoundsChanged(org_vmin, org_vmax);
  }
  LocationChanged(oldpos);
}

void SceneNode::SetScale(const Vector3& v) {
  using namespace azer;
  Vector3 org_vmin = vmin_;
  Vector3 org_vmax = vmax_;
  vmin_ = RevertTranformOnPos(vmin_);
  vmax_ = RevertTranformOnPos(vmax_);
  mutable_holder()->SetScale(v);
  vmin_ = ApplyTranformOnPos(vmin_);
  vmax_ = ApplyTranformOnPos(vmax_);
  if (vmin_ != org_vmin || vmax_ != org_vmax) {
    BoundsChanged(org_vmin, org_vmax);
  }
}

void SceneNode::SetMin(const Vector3& v) {
  Vector3 s = holder().scale();
  Vector3 pos = holder().position();

  Vector3 org_vmin = vmin_;
  vmin_ = ApplyTranformOnPos(v);

  if (vmin_ != org_vmin) {
    BoundsChanged(org_vmin, vmax_);
  }
}

void SceneNode::SetMax(const Vector3& v) {
  Vector3 s = holder().scale();
  Vector3 pos = holder().position();
  Vector3 org_vmax = vmax_;
  vmax_ = ApplyTranformOnPos(v);
  if (vmax_ != org_vmax) {
    BoundsChanged(vmin_, org_vmax);
  }
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
                    OnSceneNodeLocationChanged(this, orgpos));
}

void SceneNode::OrientationChanged(const Quaternion& origin_orient) {
  FOR_EACH_OBSERVER(SceneNodeObserver, 
                    observers_, 
                    OnSceneNodeOrientationChanged(this, origin_orient));
}

void SceneNode::BoundsChanged(const Vector3& orgmin, const Vector3& orgmax) {
  if (parent()) {
    parent()->CalcChildrenBoundingVector();
  }

  FOR_EACH_OBSERVER(SceneNodeObserver, 
                    observers_, 
                    OnSceneNodeBoundsChanged(this, orgmin, orgmax));
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

const char* SceneNodeName(int32 type) {
  switch (type) {
    case kSceneNode: return "SceneNode";
    case kEnvSceneNode: return "EnvNode";
    case kMeshSceneNode: return "MeshNode";
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
}  // namespace lord
