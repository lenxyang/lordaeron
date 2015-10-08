#include "lordaeron/ui/scene_tree_view.h"

#include "ui/views/layout/fill_layout.h"
#include "lordaeron/context.h"
#include "lordaeron/scene/scene_tree_model.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
SceneTreeView::SceneTreeView(SceneNodePtr node) {
  InitUI();
  tree_model_.reset(new SceneTreeModel(node));
  tree_view_->SetModel(tree_model_);
}

SceneTreeView::~SceneTreeView() {
}

void SceneTreeView::InitUI() {
  tree_view_ = new views::TreeView;
  tree_view_->SetRootShown(false);
  tree_view_->SetController(this);
  AddChildView(tree_view_);
  SetLayoutManager(new views::FillLayout);
}

void SceneTreeView::Init() {
}

void SceneTreeView::OnTreeViewSelectionChanged(views::TreeView* tree_view) {
}

bool SceneTreeView::CanEdit(views::TreeView* tree_view, ui::TreeModelNode* node) {
  return false;
}

// class SceneTreeWindow
SceneTreeWindow::SceneTreeWindow(SceneNodePtr node) 
    : view_(NULL) {
  view_ = new SceneTreeView(node);
  AddChildView(view_);
  SetLayoutManager(new views::FillLayout);
}

SceneTreeWindow::~SceneTreeWindow() {
}
}  // namespace lord
