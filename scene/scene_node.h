#pragma once

#include <map>
#include <vector>

#include "base/memory/ref_counted.h"
#include "base/observer_list.h"
#include "azer/render/render.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/scene/scene_node_observer.h"

namespace lord {
class Light;
class SceneNode;
class SceneNodeData;
class SceneNodeObserver;
typedef scoped_refptr<SceneNode> SceneNodePtr;
typedef std::vector<SceneNodePtr> SceneNodes;

enum SceneNodeType {
  kSceneNode,
  kEnvSceneNode,
  kObjectSceneNode,
  kLampSceneNode,
  kCameraSceneNode,
  kTerrainTileSceneNode,
};

class SceneNodeData : public SceneNodeObserver {
 public:
  SceneNodeData(SceneNode* node);
  ~SceneNodeData();

  void reset();
  void AttachMesh(azer::MeshPtr mesh);
  azer::Mesh* GetMesh();

  void AttachLight(Light* light);
  Light* light();

  void SetSceneNode(SceneNode* node);
 private:
  // override SceneNodeObserver
  void OnNodeOrientChanged(SceneNode* node, const azer::Quaternion& prev) override;
  void OnNodeLocationChanged(SceneNode* node, const azer::Vector3& prev) override;
  azer::MeshPtr mesh_;
  LightPtr light_;
  SceneNode* node_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeData);
};

typedef scoped_refptr<SceneNodeData> SceneNodeDataPtr;

class SceneNode: public ::base::RefCounted<SceneNode> {
 public:
  SceneNode();
  explicit SceneNode(const std::string& name);
  SceneNode(const std::string& name, SceneNode* parent);
  SceneNode(const std::string& name, SceneNodeType type, SceneNode* parent);
  ~SceneNode();
  
  // attributes
  void set_visible(bool visible) { visible_ = visible;}
  bool visible() const { return visible_;}

  void set_draw_bounding_volumn(bool b);
  bool is_draw_bounding_volumn() const;
  void set_pickable(bool pickable) { pickable_ = pickable;}
  bool pickable() const { return pickable_;}
  void set_picked(bool picked) { picked_ = picked;}
  bool picked() const { return picked_;}

  void set_shadow_caster(bool v) {shadow_caster_ = v;}
  bool shadow_caster() const { return shadow_caster_;}

  const azer::Vector3& local_vmin() const { return local_vmin_;}
  const azer::Vector3& local_vmax() const { return local_vmax_;}
  void SetLocalBounds(const azer::Vector3& vmin, const azer::Vector3& vmax);

  void AddChild(SceneNode* child);
  void RemoveChild(SceneNode* child);
  bool has_child() const { return !children_.empty();}
  int32 child_count() const { return children_.size();}
  SceneNode* child_at(int32 index) { return children_.at(index);}
  bool HasAncestor(SceneNode* node) const;
  SceneNodes* mutable_children() { return &children_;}

  // absolute path: //level1/level2/level3
  // relative path: level1/level2/level3
  // relative path: ../level1/level2
  SceneNode* GetNode(const std::string& path);
  void CreatePathRecusive(const std::string& path);
  bool AddChildAtPath(const std::string& parent, SceneNode* node);
  SceneNode* RemoveChildAtPath(const std::string& path);

  void set_name(const std::string& name);
  const std::string& name() const { return name_;}
  std::string path() const;

  SceneNode* root();
  SceneNode* parent() { return parent_;}
  const SceneNode* parent() const { return parent_;}
  const SceneNodes& children() const { return children_;}
  SceneNodes& children() { return children_;}

  // tranform
  const azer::TransformHolder& holder() const { return holder_;}
  void SetPosition(const azer::Vector3& pos);
  const azer::Vector3& position() const { return holder().position();}
  azer::Vector3 GetWorldPosition() const;
  
  void SetScale(const azer::Vector3& v);
  const azer::Vector3& scale() const { return holder().scale();}

  void set_orientation(const azer::Quaternion& q);
  const azer::Quaternion& orientation() const { return holder().orientation();}
  void pitch(const azer::Radians angle);
  void pitch(const azer::Degree angle);
  void yaw(const azer::Radians angle);
  void yaw(const azer::Degree angle);
  void roll(const azer::Radians angle);
  void roll(const azer::Degree angle);
  void rotate(const azer::Vector3& axis, const azer::Degree angle);
  void rotate(const azer::Vector3& axis, const azer::Radians angle);

  std::string print_info();

  SceneNodeType type() const;
  void SetNodeType(SceneNodeType type);
  const SceneNodeData* data() const { return data_.get();}
  SceneNodeData* mutable_data() { return data_.get();}

  void set_user_data(void* data) { user_data_ = data;}
  void* user_data() {return user_data_;}
  const void* user_data() const {return user_data_;}
  void AddAttr(const std::string& name, const std::string& value);
  std::string GetAttr(const std::string& name) const;

  void AddObserver(SceneNodeObserver* observer); 
  void RemoveObserver(SceneNodeObserver* observer);
  bool HasObserver(SceneNodeObserver* observer);
 protected:
  void InitMember();
  void BoundsChanged(const azer::Vector3& orgmin, const azer::Vector3& orgmax);
  void ScaleChanged(const azer::Vector3& org_scale);
  void LocationChanged(const azer::Vector3& orgpos);
  void OrientationChanged(const azer::Quaternion& origin_orient);
  void NotifyParentBoundsChanged();
  void UpdateBoundsByChildren();
  azer::TransformHolder* mutable_holder() { return &holder_;}

  SceneNode* GetLocalChild(const std::string& name);
  void print_info(std::string* str, int depth, SceneNode* node);
  // attributes
  bool visible_;
  bool pickable_;
  bool picked_;
  bool shadow_caster_;
  bool draw_bounding_;
  SceneNode* parent_;
  SceneNodes children_;
  std::string name_;
  SceneNodeType type_;
  void* user_data_;
  scoped_ptr<SceneNodeData> data_;
  azer::TransformHolder holder_;
  std::map<std::string, std::string> attributes_;

  // bounding
  azer::Vector3 local_vmin_;
  azer::Vector3 local_vmax_;
  ObserverList<SceneNodeObserver> observers_;
  DISALLOW_COPY_AND_ASSIGN(SceneNode);
};

const char* SceneNodeName(int32 type);
azer::Matrix4 GenWorldMatrixForSceneNode(SceneNode* node);
void GetSceneNodeBounds(SceneNode* node, azer::Vector3* vmin, azer::Vector3* vmax);
}  // namespace lord
