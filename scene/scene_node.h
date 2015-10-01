#pragma once

#include "base/memory/ref_counted.h"
#include "azer/render/render.h"

namespace lord {
class SceneNode;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class SceneNode: public ::base::RefCounted<SceneNode> {
 public:
  SceneNode();
  explicit SceneNode(const std::string& name);
  ~SceneNode();

  typedef std::vector<SceneNodePtr> SceneNodes;
  
  void set_visible(bool visible) { visible_ = visible;}
  bool is_visible() const { return visible_;}

  void AddChild(SceneNodePtr child);
  void RemoveChild(SceneNodePtr child);
  bool has_child() const { return !children_.empty();} 
  bool HasAncestor(SceneNode* node) const;

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
 protected:
  SceneNodePtr GetLocalChild(const std::string& name);
  void print_info(std::string* str, int depth, SceneNode* node);
  bool visible_;

  SceneNode* parent_;
  SceneNodes children_;
  std::string name_;
  azer::TransformHolder holder_;
  DISALLOW_COPY_AND_ASSIGN(SceneNode);
};

}  // namespace lord
