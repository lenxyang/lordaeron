#pragma once

#include "base/memory/ref_counted.h"
#include "azer/render/render.h"

namespace lord {
class SceneContext;
class SceneNode;
class SceneNodeData;
typedef scoped_refptr<SceneContext> SceneContextPtr;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class SceneNode: public ::base::RefCounted<SceneNode> {
 public:
  enum Type {
    kEmptyNode,
    kMeshNode,
    kLampNode,
    kCameraNode,
    kTerrainTileNode,
  };

  SceneNode();
  SceneNode(SceneContextPtr context);
  explicit SceneNode(const std::string& name);
  ~SceneNode();

  typedef std::vector<SceneNodePtr> SceneNodes;
  
  void set_visible(bool visible) { visible_ = visible;}
  bool is_visible() const { return visible_;}

  void AddChild(SceneNodePtr child);
  void RemoveChild(SceneNodePtr child);
  bool has_child() const { return !children_.empty();}
  int32 child_count() const { return children_.size();}
  SceneNodePtr child_at(int32 index) { return children_.at(index);}
  bool HasAncestor(SceneNode* node) const;
  SceneNodes* mutable_children() { return &children_;}

  // absolute path: //level1/level2/level3
  // relative path: level1/level2/level3
  // relative path: ../level1/level2
  SceneNodePtr GetNode(const std::string& path);
  void CreatePathRecusive(const std::string& path);
  bool AddChildAtPath(const std::string& parent, SceneNodePtr node);
  SceneNodePtr RemoveChildAtPath(const std::string& path);

  void set_name(const std::string& name);
  const std::string& name() const { return name_;}
  std::string path() const;

  SceneNode* root();
  SceneNode* parent() { return parent_;}
  const SceneNode* parent() const { return parent_;}
  const SceneNodes& children() const { return children_;}
  SceneNodes& children() { return children_;}

  const azer::TransformHolder& holder() const { return holder_;}
  azer::TransformHolder* mutable_holder() { return &holder_;}
  std::string print_info();

  Type type() const;
  const SceneNodeData* data() const { return data_.get();}
  SceneNodeData* mutable_data() { return data_.get();}

  void set_user_data(void* data) { user_data_ = data;}
  void* user_data() {return user_data_;}
  const void* user_data() const {return user_data_;}

  azer::Matrix4* mutable_world() { return &world_;}
  const azer::Matrix4& world() const { return world_;}

  SceneContextPtr context();
 protected:
  SceneNodePtr GetLocalChild(const std::string& name);
  void print_info(std::string* str, int depth, SceneNode* node);
  bool visible_;

  SceneNode* parent_;
  SceneNodes children_;
  std::string name_;
  void* user_data_;
  scoped_ptr<SceneNodeData> data_;
  azer::Matrix4 world_;
  azer::TransformHolder holder_;

  SceneContextPtr context_;
  DISALLOW_COPY_AND_ASSIGN(SceneNode);
};

}  // namespace lord
