#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/tree/tree_view.h"
#include "ui/views/controls/tree/tree_view_controller.h"
#include "nelf/nelf.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {

class SceneTreeModel;
class SceneTreeView : public views::View,
                      public views::TreeViewController {
 public:
  SceneTreeView(SceneNodePtr node);
  ~SceneTreeView();

  // override from views::TreeViewController
  void OnTreeViewSelectionChanged(views::TreeView* tree_view) override;
  bool CanEdit(views::TreeView* tree_view, ui::TreeModelNode* node) override;
 private:
  void InitUI();
  void Init();
  scoped_ptr<SceneTreeModel> tree_model_;
  views::TreeView* tree_view_;
  DISALLOW_COPY_AND_ASSIGN(SceneTreeView);
};

class SceneTreeWindow : public nelf::Window {
 public:
  SceneTreeWindow(nelf::Window* window, SceneNodePtr node);
  ~SceneTreeWindow() override;
 private:
  SceneTreeView* view_;
  DISALLOW_COPY_AND_ASSIGN(SceneTreeWindow);
};
}  // namespace lord
