#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/tree/tree_view.h"
#include "ui/views/controls/tree/tree_view_controller.h"
#include "nelf/nelf.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/window.h"

namespace lord {

class SceneTreeModel;
class SceneTreeView : public views::View,
                      public nelf::TreeViewController {
 public:
  SceneTreeView(SceneNodePtr node);
  ~SceneTreeView();

  // override from views::TreeViewController
  void OnTreeViewSelectionChanged(nelf::TreeView* tree_view) override;
  bool CanEdit(nelf::TreeView* tree_view, ui::TreeModelNode* node) override;
 private:
  void InitUI();
  void Init();
  scoped_ptr<SceneTreeModel> tree_model_;
  nelf::CollapsedBasedTreeView* tree_view_;
  DISALLOW_COPY_AND_ASSIGN(SceneTreeView);
};

class SceneTreeWindow : public Window {
 public:
  SceneTreeWindow(const gfx::Rect& init_bounds,  nelf::Window* window);
  ~SceneTreeWindow() override;

  void SetSceneNode(SceneNodePtr node);
 private:
  SceneTreeView* view_;
  DISALLOW_COPY_AND_ASSIGN(SceneTreeWindow);
};
}  // namespace lord
