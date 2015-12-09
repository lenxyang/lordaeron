#include "lordaeron/ui/scene_tree_view.h"

#include "base/strings/utf_string_conversions.h"
#include "base/lazy_instance.h"
#include "ui/views/layout/fill_layout.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"
#include "lordaeron/ui/iconset.h"
#include "lordaeron/ui/scene_tree_view_lineitem.h"
#include "lordaeron/scene/scene_tree_model.h"

namespace lord {
namespace {
base::LazyInstance<SceneTreeViewLineItemViewCreator> node_creator
= LAZY_INSTANCE_INITIALIZER;
}  // namespace

const char SceneTreeView::kViewClassName[] = "nelf::SceneTreeView";

SceneTreeView::SceneTreeView(SceneNode* node) {
  InitUI();
  tree_model_.reset(new SceneTreeModel(node));
  tree_view_->SetModel(tree_model_.get());
}

SceneTreeView::~SceneTreeView() {
}

const char* SceneTreeView::GetClassName() const {
  return kViewClassName;
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
nelf::TabbedWindow* CreateSceneTreeViewWindow(
    const gfx::Rect& bounds, SceneNode* root, nelf::Window* parent) {
  SceneTreeView* view = new SceneTreeView(root);
  nelf::Pane* pane = new nelf::Pane();
  pane->GetContents()->AddChildView(view);
  pane->GetContents()->SetLayoutManager(new views::FillLayout);
  pane->SetTitle(::base::UTF8ToUTF16("scene"));
  nelf::TabbedWindow* window = new nelf::TabbedWindow(bounds, parent->mainframe());
  window->AddPane(pane);
  window->Init();
  window->Show();
  return window;
}
}  // namespace lord
