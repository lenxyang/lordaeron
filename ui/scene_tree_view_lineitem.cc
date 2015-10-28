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
#include "lordaeron/context.h"
#include "lordaeron/scene/scene_tree_model.h"
#include "lordaeron/ui/iconset.h"
#include "lordaeron/ui/scene_node_control_panel.h"

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
  Context* ctx = Context::instance();
  SceneTreeModelNode* model_node =
      dynamic_cast<SceneTreeModelNode*>(node->model_node());

  control_panel_ = new SceneNodeControPanel(model_node->scene_node());
  AddChildView(control_panel_);
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
  CalcLayout();
}

void SceneTreeViewLineItemView::OnNodeChanged()  {
  UpdateNodeInfo();
}
void SceneTreeViewLineItemView::CalcLayout() {
  views::Label* label = new views::Label(node_->model_node()->GetTitle());
  nelf::TreeView* tree_view = node_->tree_view();
  ui::TreeModel* model = tree_view->model();
  const gfx::FontList& fontlist = node_->tree_view()->GetFontList();
  label->SetFontList(fontlist);
  {
    const gfx::ImageSkia closed_icon = tree_view->closed_icon();
    int x = CalcLeftMargin();
    int y = (GetContentsBounds().height() - closed_icon.width()) * 0.5;
    expand_bounds_.set_origin(gfx::Point(x, y));
    expand_bounds_.set_size(closed_icon.size());
    if (model->GetChildCount(node_->model_node()) == 0)
      expand_bounds_.set_width(0);
  }

  if (icon_.width() > 0) {
    int x = expand_bounds_.right();
    if (expand_bounds_.width() > 0)
      x += kImageMargin;
    int y = (GetContentsBounds().height() - icon_.width()) * 0.5;
    icon_bounds_.set_x(x);
    icon_bounds_.set_y(y);
    icon_bounds_.set_size(icon_.size());
  }

  int x = icon_bounds_.right();
  if (icon_bounds_.width() > 0)
    x += kImageMargin;
  text_bounds_.set_origin(gfx::Point(x, kTextVerticalPadding));
  text_bounds_.set_size(label->GetPreferredSize());

  int right_pos = text_bounds_.right();

  {
    gfx::Size panel_prefsize = control_panel_->GetPreferredSize();
    int x = right_pos + 10;
    int y = (height() - panel_prefsize.height()) * 0.5f;
    control_panel_->SetPosition(gfx::Point(x, y));
    control_panel_->SetSize(panel_prefsize);
  }

  gfx::Size pref_size = preferred_size_;
  preferred_size_ = gfx::Size(right_pos, line_height_);
  if (pref_size != preferred_size_) {
    PreferredSizeChanged();
    SchedulePaint();
  }
}

const char* SceneTreeViewLineItemView::GetClassName() const {
  return kViewClassName;
}

void SceneTreeViewLineItemView::Layout() {
  CalcLayout();

  nelf::CollapsedBasedTreeView::LineItemView::Layout();
}

void SceneTreeViewLineItemView::OnPaint(gfx::Canvas* canvas) {
  nelf::CollapsedBasedTreeView* tree = node_->tree_view();
  Theme* theme = nelf::GetThemeFromNativeTheme(GetNativeTheme());
  if (expand_bounds_.width() > 0) {
    gfx::ImageSkia expand_icon;
    if (node_->collapsed()) {
      expand_icon = tree->open_icon();
    } else {
      expand_icon = tree->closed_icon();
    }
    canvas->DrawImageInt(expand_icon, expand_bounds_.x(), expand_bounds_.y());
  }

  if (icon_.width() > 0) {
    canvas->DrawImageInt(icon_, icon_bounds_.x(), icon_bounds_.y());
  }

  SkColor text_color;
  bool selected = (tree->GetSelectedNode() == node_);
  if (selected) {
    text_color = theme->GetColor(Theme::kColorId_TreeViewTextHighlightColor);
  } else {
    text_color = theme->GetColor(Theme::kColorId_TreeViewTextColor);
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

void SceneTreeViewLineItemView::ExpandOrCollapse() {
  if (node_->collapsed()) {
    node_->Expand();
  } else {
    node_->Collapse();
  }
}

void SceneTreeViewLineItemView::OnVisibleBoundsChanged() {
  Layout();
}

void SceneTreeViewLineItemView::VisibilityChanged(View* starting_from, 
                                                  bool is_visible) {
  if (is_visible)
    Layout();
}

bool SceneTreeViewLineItemView::OnMousePressed(const ui::MouseEvent& event)  {
  if (expand_bounds_.Contains(event.location())) {
    ExpandOrCollapse();
  } else if (event.GetClickCount() == 2 && event.IsOnlyLeftMouseButton()) {
    ExpandOrCollapse();
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

