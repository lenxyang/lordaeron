#include "lordaeron/ui/scene_node_property_pane.h"

#include "base/strings/utf_string_conversions.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using base::UTF8ToUTF16;
namespace {
int LayoutControlRightAlign(views::View* parent, views::View* view, int32 y) {
  const gfx::Rect bounds = std::move(parent->GetContentsBounds());
  view->SizeToPreferredSize();
  view->SetPosition(gfx::Point(bounds.right() - view->width(), y));
  return view->bounds().bottom(); 
}

int LayoutControlLeftAlign(views::View* parent, views::View* view, int32 y) {
  const gfx::Rect bounds = std::move(parent->GetContentsBounds());
  view->SizeToPreferredSize();
  view->SetPosition(gfx::Point(bounds.x(), y));
  return view->bounds().bottom(); 
}
}

// class TransformContents
const char TransformContents::kViewClassName[]
= "lor::TransformContents";
TransformContents::TransformContents(SceneNode* node) 
    : node_(node) {
  namelabel_ = new views::Label(UTF8ToUTF16(node->name()));
  AddChildView(namelabel_);
  namefield_ = new views::Textfield;
  AddChildView(namefield_);
  
  position_control_ = new VectorControl(node->position());
  AddChildView(position_control_);
  orientation_control_ = new VectorControl(node->holder().orientation());
  AddChildView(orientation_control_);
  scale_control_ = new VectorControl(node->holder().scale());
  AddChildView(scale_control_);

  position_label_ = new views::Label(UTF8ToUTF16("Position"));
  AddChildView(position_label_);
  orientation_label_ = new views::Label(UTF8ToUTF16("Orientation"));
  AddChildView(orientation_label_);
  scale_label_ = new views::Label(UTF8ToUTF16("Scale"));
  AddChildView(scale_label_);
  SetBorder(views::Border::CreateEmptyBorder(4, 4, 4, 4));
}

TransformContents::~TransformContents() {}

const char* TransformContents::GetClassName() const {
  return kViewClassName;
}

gfx::Size TransformContents::GetPreferredSize() const {
  return gfx::Size(360, 240);
}

void TransformContents::Layout() {
  const gfx::Rect bounds = std::move(GetContentsBounds());
  int32 x = bounds.x();
  int32 y = GetContentsBounds().y();
  const int32 kLinePadding = 1;
  LayoutControlLeftAlign(this, position_label_, y);
  y = LayoutControlRightAlign(position_control_, y);
  y += kLinePadding;

  LayoutControlLeftAlign(this, orientation_label_, y);
  y = LayoutControlRightAlign(orientation_control_, y);
  y += kLinePadding;

  LayoutControlLeftAlign(this, scale_label_, y);
  y = LayoutControlRightAlign(scale_control_, y);
  y += kLinePadding;
}

void TransformContents::OnVectorChanged(VectorControl* control) {
  if (control == position_control_) {
    node_->SetPosition(control->GetVec3Value());
  } else if (control == orientation_control_) {
    node_->set_orientation(control->GetQuaternionValue());
  } else if (control == scale_control_) {
    node_->SetScale(control->GetVec3Value());
  } else {
    CHECK(false);
  }
}

// 
const char SceneNodePropertyPane::kViewClassName[] = "nelf::SceneNodePropertyPane";
SceneNodePropertyPane::SceneNodePropertyPane(SceneNode* node)
    : nelf::CollapseView(::base::UTF8ToUTF16("SceneNode")) {
  contents_ = new TransformContents(node);
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
  }
  SetCollapseSize(size);
  nelf::CollapseView::Layout();
}
}  // namespace lord
