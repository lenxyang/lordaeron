#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/tree/tree_view.h"
#include "ui/views/controls/tree/tree_view_controller.h"
#include "nelf/nelf.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/window/window.h"

namespace lord {

class SceneTreeModel;
class SceneTreeView : public views::View,
                      public nelf::TreeViewController {
 public:
  static const char kViewClassName[];
  SceneTreeView(SceneNode* node);
  ~SceneTreeView();

  const char* GetClassName() const override;
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

nelf::TabbedWindow* CreateSceneTreeViewWindow(
    const gfx::Rect& bounds, SceneNode* root, nelf::Window* window);
}  // namespace lord
