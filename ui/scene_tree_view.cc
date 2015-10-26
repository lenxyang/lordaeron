#include "lordaeron/ui/scene_tree_view.h"

#include "base/strings/utf_string_conversions.h"
#include "base/lazy_instance.h"
#include "ui/views/layout/fill_layout.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"
#include "lordaeron/ui/iconset.h"
#include "lordaeron/ui/scene_tree_view_lineitem.h"
#include "lordaeron/scene/scene_tree_model.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
namespace {
base::LazyInstance<SceneTreeViewLineItemViewCreator> node_creator
= LAZY_INSTANCE_INITIALIZER;
}  // namespace

SceneTreeView::SceneTreeView(SceneNodePtr node) {
  InitUI();
  tree_model_.reset(new SceneTreeModel(node));
  tree_view_->SetModel(tree_model_.get());
}

SceneTreeView::~SceneTreeView() {
}

void SceneTreeView::InitUI() {
  Context* ctx = Context::instance();
  tree_view_ = new nelf::CollapsedBasedTreeView;
  tree_view_->SetNodeCreator(node_creator.Pointer());
  tree_view_->set_closed_icon(ctx->GetIcon(Iconset::kIconCollapse));
  tree_view_->set_open_icon(ctx->GetIcon(Iconset::kIconExpand));
  tree_view_->SetRootShown(false);
  tree_view_->SetController(this);
  AddChildView(tree_view_);
  SetLayoutManager(new views::FillLayout);
}

void SceneTreeView::Init() {
}

void SceneTreeView::OnTreeViewSelectionChanged(nelf::TreeView* tree_view) {
}

bool SceneTreeView::CanEdit(nelf::TreeView* tree_view, ui::TreeModelNode* node) {
  return false;
}

// class SceneTreeWindow
SceneTreeWindow::SceneTreeWindow(const gfx::Rect& init_bounds, nelf::Window* window)
    : Window(init_bounds, window),
      view_(NULL) {
  SetLayoutManager(new views::FillLayout);
  SetTitle(base::UTF8ToUTF16("Scene"));
  Context* ctx = Context::instance();
  set_show_icon(true);
  set_window_icon(ctx->GetIcon(Iconset::kIconWindowExpanded));
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
