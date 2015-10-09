#include "lordaeron/ui/scene_tree_view.h"

#include "base/strings/utf_string_conversions.h"
#include "ui/views/layout/fill_layout.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"
#include "lordaeron/scene/scene_tree_model.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
SceneTreeView::SceneTreeView(SceneNodePtr node) {
  InitUI();

  Context* context = Context::instance();
  std::vector<gfx::ImageSkia> icons;
  int32 toolbar_id[] = {
    IDR_ICON_SCENE_UNKNOWN,
    IDR_ICON_SCENE_MESH,
    IDR_ICON_SCENE_LAMP,
    IDR_ICON_SCENE_SUN,
  };
  for (int i = 0; i < arraysize(toolbar_id); ++i) {
    int32 id = toolbar_id[i];
    icons.push_back(*(context->resource_bundle()->GetImageSkiaNamed(id)));
  }
  tree_model_.reset(new SceneTreeModel(node));
  tree_model_->SetImageVec(icons);
  tree_view_->SetModel(tree_model_.get());
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
SceneTreeWindow::SceneTreeWindow(const gfx::Rect& init_bounds, nelf::Window* window)
    : Window(init_bounds, window),
      view_(NULL) {
  SetLayoutManager(new views::FillLayout);
  set_show_title(true);
  SetTitle(base::UTF8ToUTF16("Scene"));
}

SceneTreeWindow::~SceneTreeWindow() {
}

void SceneTreeWindow::SetSceneNode(SceneNodePtr node) {
  if (view_)
    delete view_;
  view_ = new SceneTreeView(node);
  AddChildView(view_);
}
}  // namespace lord
