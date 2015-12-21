#pragma once

#include <vector>
#include <map>

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/observer_list.h"
#include "base/strings/string16.h"

#include "ui/base/models/tree_model.h"
#include "ui/gfx/image/image_skia.h"
#include "azer/base/repository_node.h"

namespace lord {
class RepositoryModelNode : public ui::TreeModelNode {
 public:
  typedef std::vector<RepositoryModelNode*> Assets;
  explicit RepositoryModelNode(azer::RepositoryNodePtr node);
  virtual ~RepositoryModelNode();

  // override from ui::TreeModelNode
  const base::string16& GetTitle() const override;

  azer::RepositoryNodePtr repository_node() { return node_;}
  void AddChild(RepositoryModelNode* item) { children_.push_back(item);}
  RepositoryModelNode* child_at(int index);
  int32 child_count() const { return static_cast<int32>(children_.size());}
  int32 GetIndexOf(RepositoryModelNode* item) const;
 private:
  azer::RepositoryNodePtr node_;
  std::vector<RepositoryModelNode*> children_;
  DISALLOW_COPY_AND_ASSIGN(RepositoryModelNode);
};

// assets window's data provider
class RepositoryModel : public ui::TreeModel {
 public:
  RepositoryModel(const base::string16& name, azer::RepositoryNodePtr root);
  virtual ~RepositoryModel();

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
 protected:
  RepositoryModelNode* BuildTree(azer::RepositoryNodePtr node);
  std::vector<gfx::ImageSkia> icons_;
  azer::RepositoryNodePtr root_;
  base::string16 name_;
  ObserverList<ui::TreeModelObserver> observers_;
  std::vector<RepositoryModelNode*> items_;
  DISALLOW_COPY_AND_ASSIGN(RepositoryModel);
};

}  // namespace lord
