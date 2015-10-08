#pragma once

#include <vector>
#include <map>

#include "base/strings/string16.h"
#include "ui/base/models/tree_model.h"
#include "ui/gfx/image/image_skia.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
class SceneTreeModelNode : public ui::TreeModelNode {
 public:
  explicit SceneTreeModelNode(SceneNodePtr node);
  ~SceneTreeModelNode();

  SceneNodePtr scene_node() { return node_;}
  void AddChild(SceneTreeModelNode* item) { children_.push_back(item);}
  SceneTreeModelNode* child_at(int index);
  int32 child_count() const { return static_cast<int32>(children_.size());}
  int32 GetIndexOf(SceneTreeModelNode* item) const;

  // override from ui::TreeModelNode
  const base::string16& GetTitle() const override;
 private:
  SceneNodePtr node_;
  std::vector<SceneTreeModelNode*> children_;
  DISALLOW_COPY_AND_ASSIGN(SceneTreeModelNode);
};

class SceneTreeModel : public ui::TreeModel {
 public:
  explicit SceneTreeModel(SceneNodePtr root);
  ~SceneTreeModel();

  const ::base::string16 name() const { return name_;}
  void SetImageVec(const std::vector<gfx::ImageSkia>& icons);

  // override from ui::TreeModel
  ui::TreeModelNode* GetRoot() override;
  int GetChildCount(ui::TreeModelNode* parent) override;
  ui::TreeModelNode* GetChild(ui::TreeModelNode* parent, int index) override;
  int GetIndexOf(ui::TreeModelNode* parent, ui::TreeModelNode* child) override;
  ui::TreeModelNode* GetParent(ui::TreeModelNode* node) override;
  void GetIcons(std::vector<gfx::ImageSkia>* icons) override;
  int GetIconIndex(ui::TreeModelNode* node) override;

  void AddObserver(ui::TreeModelObserver* observer) override;
  void RemoveObserver(ui::TreeModelObserver* observer) override;
 private:
  SceneTreeModelNode* BuildTree(SceneNodePtr node);
  std::vector<gfx::ImageSkia> icons_;
  ObserverList<ui::TreeModelObserver> observers_;
  std::vector<SceneTreeModelNode*> nodes_;
  SceneNodePtr root_;
  base::string16 name_;
  DISALLOW_COPY_AND_ASSIGN(SceneTreeModel);
};
}  // namespace lord
