#include "lordaeron/scene/scene_tree_model.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
//
SceneTreeModelNode::SceneTreeModelNode(SceneNodePtr node)
    : node_(node) {
  title_ = ::base::UTF8ToUTF16(node->name());
}

SceneTreeModelNode::~SceneTreeModelNode() {
}

const base::string16& SceneTreeModelNode::GetTitle() const {
  return title_;
}

SceneTreeModelNode* SceneTreeModelNode::child_at(int index) {
  DCHECK(index < child_count());
  return children_.at(index);
}

int32 SceneTreeModelNode::GetIndexOf(SceneTreeModelNode* item) const {
  int32 index = 0;
  for (auto iter = children_.begin(); iter != children_.end(); ++iter, ++index) {
    if (*iter == item)
      return index;
  }
  return -1;
}

// class RepositoryModel

SceneTreeModel::SceneTreeModel(SceneNodePtr root)
    : root_(root) {
  BuildTree(root_);
  name_ = ::base::UTF8ToUTF16(root_->name());
}

SceneTreeModel::~SceneTreeModel() {
  for (auto iter = nodes_.begin(); iter != nodes_.end(); ++iter) {
    delete *iter;
  }
  nodes_.clear();
}

SceneTreeModelNode* SceneTreeModel::BuildTree(SceneNodePtr node) {
  if (node->child_count() > 0) {
    SceneTreeModelNode* child = new SceneTreeModelNode(node);
    node->set_user_data(child);
    nodes_.push_back(child);

    for (auto iter = node->children().begin();
         iter != node->children().end(); ++iter) {
      SceneTreeModelNode* sub = BuildTree(*iter);
      if (sub) {
        child->AddChild(sub);
      }
    }
    return child;
  } else {
    return NULL;
  }
}

void SceneTreeModel::SetImageVec(const std::vector<gfx::ImageSkia>& icons) {
  icons_ = icons;
}

ui::TreeModelNode* SceneTreeModel::GetRoot()  {
  DCHECK(root_->user_data());
  return static_cast<ui::TreeModelNode*>(root_->user_data());
}

int SceneTreeModel::GetChildCount(ui::TreeModelNode* n)  {
  SceneTreeModelNode* node = dynamic_cast<SceneTreeModelNode*>(n);
  return node->child_count();
}

ui::TreeModelNode* SceneTreeModel::GetChild(ui::TreeModelNode* n, int index)  {
  SceneTreeModelNode* node = dynamic_cast<SceneTreeModelNode*>(n);
  return node->child_at(index);
}

int SceneTreeModel::GetIndexOf(ui::TreeModelNode* p, ui::TreeModelNode* c) {
  SceneTreeModelNode* node = dynamic_cast<SceneTreeModelNode*>(p);
  SceneTreeModelNode* child = dynamic_cast<SceneTreeModelNode*>(c);
  return node->GetIndexOf(child);
}

ui::TreeModelNode* SceneTreeModel::GetParent(ui::TreeModelNode* n)  {
  SceneTreeModelNode* node = dynamic_cast<SceneTreeModelNode*>(n);
  SceneNodePtr rnode = node->scene_node();
  SceneNode* parent = rnode->parent();
  if (parent) {
    DCHECK(parent->user_data());
    return static_cast<SceneTreeModelNode*>(parent->user_data());
  } else {
    return NULL;
  }
}

void SceneTreeModel::GetIcons(std::vector<gfx::ImageSkia>* icons)  {
  *icons = icons_;
}

int SceneTreeModel::GetIconIndex(ui::TreeModelNode* node)  {
  return 0;
}

void SceneTreeModel::AddObserver(ui::TreeModelObserver* observer) {
  observers_.AddObserver(observer);
}

void SceneTreeModel::RemoveObserver(ui::TreeModelObserver* observer) {
  observers_.RemoveObserver(observer);
}
}  // namespace lord
