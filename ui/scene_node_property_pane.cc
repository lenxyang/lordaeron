#include "lordaeron/ui/scene_node_property_pane.h"

#include "base/strings/utf_string_conversions.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/layout_util.h"

namespace lord {
using base::UTF8ToUTF16;

const char SceneNodeContents::kViewClassName[] = "lord::SceneNodeContents";
SceneNodeContents::SceneNodeContents(SceneNode* node)
    : node_(node) {
  name_label_ = new views::Label(UTF8ToUTF16("Name"));
  AddChildView(name_label_);
  name_textfield_ = new views::Textfield();
  name_textfield_->SetText(::base::UTF8ToUTF16(node->name()));
  AddChildView(name_textfield_);

  vmin_label_ = new views::Label(UTF8ToUTF16("Bounding Min"));
  AddChildView(vmin_label_);
  vmin_control_ = new VectorControl(node->vmin());
  vmin_control_->SetReadOnly(true);
  AddChildView(vmin_control_);
  vmax_label_ = new views::Label(UTF8ToUTF16("Bounding Max"));
  AddChildView(vmax_label_);
  vmax_control_ = new VectorControl(node->vmax());
  vmax_control_->SetReadOnly(true);
  AddChildView(vmax_control_);
}

SceneNodeContents::~SceneNodeContents() {
}

const char* SceneNodeContents::GetClassName() const {
  return kViewClassName;
}
gfx::Size SceneNodeContents::GetPreferredSize() const {
  return gfx::Size(320, 80);
}

void SceneNodeContents::Layout() {
  const gfx::Rect bounds = std::move(GetContentsBounds());
  int32 x = bounds.x();
  const int32 kLinePadding = 3;
  int32 y = GetContentsBounds().y() + kLinePadding;
  float center = 100.0f;
  float padding = 6.0f;
  LayoutCenterLeftWithPrefSize(this, name_label_, center, y, padding);
  gfx::Size textfield_size = gfx::Size(100, name_label_->height());
  y = LayoutCenterRight(this, name_textfield_, center, y, padding, textfield_size).bottom();
  y += kLinePadding;
  
  LayoutCenterLeftWithPrefSize(this, vmin_label_, center, y, padding);
  y = LayoutCenterRightWithPrefSize(this, vmin_control_, center, y, padding).bottom();
  y += kLinePadding;

  LayoutCenterLeftWithPrefSize(this, vmax_label_, center, y, padding);
  y = LayoutCenterRightWithPrefSize(this, vmax_control_, center, y, padding).bottom();
  y += kLinePadding;
}

const char SceneNodePropertyPane::kViewClassName[] = "lord::SceneNodePropertyPane";
SceneNodePropertyPane::SceneNodePropertyPane(SceneNode* node)
    : nelf::CollapseView(::base::UTF8ToUTF16("SceneNode")) {
  contents_ = new SceneNodeContents(node);
  GetContents()->AddChildView(contents_);
  GetContents()->SetLayoutManager(new views::FillLayout);
  Expand();
}

const char* SceneNodePropertyPane::GetClassName() const {
  return kViewClassName;
}

void SceneNodePropertyPane::Layout() {
  gfx::Size size = std::move(GetPreferredSize());
  if (parent()) {
    size.set_width(parent()->GetContentsBounds().width());
    views::ScrollView* scview = dynamic_cast<views::ScrollView*>(parent());
    if (scview) {
      size.set_width(scview->GetVisibleRect().width());
    }
  }
  SetCollapseSize(size);
  nelf::CollapseView::Layout();
}
}  // namespace lord
