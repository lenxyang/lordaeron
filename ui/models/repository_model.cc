#include "lordaeron/ui/models/repository_model.h"

#include "base/strings/utf_string_conversions.h"

namespace lord {
using azer::RepositoryNodePtr;
using azer::RepositoryNode;

// 
RepositoryModelNode* RepositoryModelNode::child_at(int index) {
  DCHECK(index < child_count());
  return children_.at(index);
}

int32 RepositoryModelNode::GetIndexOf(RepositoryModelNode* item) const {
  int32 index = 0;
  for (auto iter = children_.begin(); iter != children_.end(); ++iter, ++index) {
    if (*iter == item)
      return index;
  }
  return -1;
}

// class RepositoryModelNode
RepositoryModelNode::RepositoryModelNode(azer::RepositoryNodePtr node)
    : node_(node) {
}

RepositoryModelNode::~RepositoryModelNode() {
}

const base::string16& RepositoryModelNode::GetTitle() const {
  return node_->name();
}

RepositoryModelNode* RepositoryModel::BuildTree(azer::RepositoryNodePtr node) {
  if (node->child_count() > 0) {
    RepositoryModelNode* item = new RepositoryModelNode(node);
    node->set_user_data(item);
    items_.push_back(item);

    for (auto iter = node->children().begin();
         iter != node->children().end(); ++iter) {
      RepositoryModelNode* child = BuildTree(iter->second);
      if (child) {
        item->AddChild(child);
      }
    }
    return item;
  } else {
    return NULL;
  }
}

// class RepositoryModel
RepositoryModel::RepositoryModel(const base::string16& name,
                                 azer::RepositoryNodePtr root)
    : root_(root),
      name_(name) {
  DCHECK(!root_->user_data());
  BuildTree(root_);
}

RepositoryModel::~RepositoryModel() {
  for (auto iter = items_.begin(); iter != items_.end(); ++iter) {
    delete *iter;
  }
  items_.clear();
}

void RepositoryModel::SetImageVec(const std::vector<gfx::ImageSkia>& icons) {
  icons_ = icons;
}

ui::TreeModelNode* RepositoryModel::GetRoot()  {
  DCHECK(root_->user_data());
  return static_cast<ui::TreeModelNode*>(root_->user_data());
}

int RepositoryModel::GetChildCount(ui::TreeModelNode* n)  {
  RepositoryModelNode* node = dynamic_cast<RepositoryModelNode*>(n);
  return node->child_count();
}

ui::TreeModelNode* RepositoryModel::GetChild(ui::TreeModelNode* n, int index)  {
  RepositoryModelNode* node = dynamic_cast<RepositoryModelNode*>(n);
  return node->child_at(index);
}

int RepositoryModel::GetIndexOf(ui::TreeModelNode* p, ui::TreeModelNode* c) {
  RepositoryModelNode* node = dynamic_cast<RepositoryModelNode*>(p);
  RepositoryModelNode* child = dynamic_cast<RepositoryModelNode*>(c);
  return node->GetIndexOf(child);
}

ui::TreeModelNode* RepositoryModel::GetParent(ui::TreeModelNode* n)  {
  RepositoryModelNode* node = dynamic_cast<RepositoryModelNode*>(n);
  RepositoryNodePtr rnode = node->repository_node();
  RepositoryNode* parent = rnode->parent();
  if (parent) {
    DCHECK(parent->user_data());
    return static_cast<RepositoryModelNode*>(parent->user_data());
  } else {
    return NULL;
  }
}

void RepositoryModel::GetIcons(std::vector<gfx::ImageSkia>* icons)  {
  *icons = icons_;
}

int RepositoryModel::GetIconIndex(ui::TreeModelNode* node)  {
  return 0;
}

void RepositoryModel::AddObserver(ui::TreeModelObserver* observer) {
  observers_.AddObserver(observer);
}

void RepositoryModel::RemoveObserver(ui::TreeModelObserver* observer) {
  observers_.RemoveObserver(observer);
}

}  // namespace lord
