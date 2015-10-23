#include "lordaeron/ui/scene_tree_view_lineitem.h"

#include "ui/native_theme/native_theme.h"
#include "ui/views/background.h"
#include "ui/resources/grit/ui_resources.h"
#include "ui/base/resource/resource_bundle.h"

#include "nelf/context.h"
#include "nelf/controls/tree/collapsebased_tree_view.h"
#include "nelf/controls/tree/collapsebased_tree_view_node.h"
#include "nelf/theme/native_theme.h"
#include "nelf/theme/theme.h"

namespace lord {

using nelf::Theme;

const int32 SceneTreeViewLineItemView::kTextVerticalPadding = 4;
const int32 SceneTreeViewLineItemView::kImageMargin = 3;
const int32 SceneTreeViewLineItemView::kIdent = 20;
const char SceneTreeViewLineItemView::kViewClassName[] = 
    "nelf::SceneTreeViewLineItemView";
SceneTreeViewLineItemView::SceneTreeViewLineItemView(
    nelf::CollapsedBasedTreeViewNode* node) 
    : node_(node) {
  line_height_ = (node->tree_view()->GetFontList().GetHeight()
                  + kTextVerticalPadding * 2);
  UpdateNodeInfo();
}

void SceneTreeViewLineItemView::OnNodeSelected(bool selected)  {
  SchedulePaint();
}

void SceneTreeViewLineItemView::UpdateNodeInfo() {
  nelf::CollapsedBasedTreeView* tree = node_->tree_view();
  ui::TreeModel* model = tree->model();
  int32 imgindex = model->GetIconIndex(node_->model_node());
  if (imgindex >= 0)
    icon_ = tree->GetIcon(imgindex);
  UpdatePreferredSize();
}

void SceneTreeViewLineItemView::OnNodeChanged()  {
  UpdateNodeInfo();
}

void SceneTreeViewLineItemView::UpdatePreferredSize() {
  views::Label* label = new views::Label(node_->model_node()->GetTitle());
  const gfx::FontList& fontlist = node_->tree_view()->GetFontList();
  label->SetFontList(fontlist);
  int width = CalcLeftMargin();
  if (icon_.width() > 0) {
    width += icon_.width();
    width += kImageMargin;
  }
  text_bounds_.set_origin(gfx::Point(width, kTextVerticalPadding));
  text_bounds_.set_size(label->GetPreferredSize());
  width += text_bounds_.width();
  gfx::Size pref_size = preferred_size_;
  preferred_size_ = gfx::Size(width, line_height_);
  if (pref_size != preferred_size_) {
    PreferredSizeChanged();
    SchedulePaint();
  }
}

const char* SceneTreeViewLineItemView::GetClassName() const {
  return kViewClassName;
}

void SceneTreeViewLineItemView::Layout() {
  UpdatePreferredSize();
  nelf::CollapsedBasedTreeView::LineItemView::Layout();
}

void SceneTreeViewLineItemView::OnPaint(gfx::Canvas* canvas) {
  nelf::CollapsedBasedTreeView* tree = node_->tree_view();
  SkColor text_color;
  Theme* theme = nelf::GetThemeFromNativeTheme(GetNativeTheme());
  bool selected = (tree->GetSelectedNode() == node_);
  if (selected) {
    text_color = theme->GetColor(Theme::kColorId_TreeViewTextHighlightColor);
  } else {
    text_color = theme->GetColor(Theme::kColorId_TreeViewTextColor);
  }

  gfx::Rect bounds = GetContentsBounds();
  if (icon_.width() > 0) {
    int x = CalcLeftMargin();
    int y = bounds.y() + (bounds.height() - icon_.height())/2;
    canvas->DrawImageInt(icon_, x, y);
  }

  const gfx::FontList& fontlist = node_->tree_view()->GetFontList();
  canvas->DrawStringRect(node_->model_node()->GetTitle(), fontlist,
                         text_color, text_bounds_);
}

void SceneTreeViewLineItemView::PreferredSizeChanged() {
}

void SceneTreeViewLineItemView::OnBoundsChanged(
    const gfx::Rect& previous_bounds) {
}

int32 SceneTreeViewLineItemView::CalcLeftMargin() const {
  return node_->depth() * kIdent;
}

gfx::Size SceneTreeViewLineItemView::GetPreferredSize() const {
  return preferred_size_;
}

bool SceneTreeViewLineItemView::OnMousePressed(const ui::MouseEvent& event)  {
  if (event.GetClickCount() == 2 && event.IsOnlyLeftMouseButton()) {
    if (node_->collapsed()) {
      node_->Expand();
    } else {
      node_->Collapse();
    }
  }
  node_->Select();
  return true;
}

void SceneTreeViewLineItemView::OnNativeThemeChanged(
    const ui::NativeTheme* nm)   {
}

void SceneTreeViewLineItemView::LayoutEditor(views::Textfield* editor) {
  editor->SetBoundsRect(text_bounds_);
  AddChildView(editor);
}

void SceneTreeViewLineItemView::OnEditBegin(views::Textfield* editor) {
}

void SceneTreeViewLineItemView::OnEditEnd(views::Textfield* editor, bool commit) {
  if (commit) {
    nelf::CollapsedBasedTreeView* tree = node_->tree_view();
    tree->model()->SetTitle(node_->model_node(), editor->text());
  }
}
}

