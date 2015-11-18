#include "lordaeron/ui/controls/collapse_view.h"

#include "ui/views/border.h"
#include "ui/views/background.h"

namespace lord {
const SkColor CollapseHead::kBorderColor = SkColorSetRGB(100, 100, 100);
const SkColor CollapseHead::kBackgroundColor = SkColorSetRGB(62, 62, 62);
const SkColor CollapseHead::kTextColor = SkColorSetRGB(235, 235, 235);

const char CollapseHead::kViewClassName[] = "lord::CollapseHead";
CollapseHead::CollapseHead(CollapseView* view)
    : collapse_(view) {
  set_background(views::Background::CreateSolidBackground(kBackgroundColor));
  SetBorder(views::Border::CreateSolidSidedBorder(1, 1, 1, 1, kBackgroundColor));
  label_ = new views::Label;
  AddChildView(label_);
  label_->SetAutoColorReadabilityEnabled(false);
  label_->SetEnabledColor(kTextColor);
}

CollapseHead::~CollapseHead() {
}

const char* CollapseHead::GetClassName() const {
  return kViewClassName;
}

bool CollapseHead::OnMousePressed(const ui::MouseEvent& event) {
  return false;
}

void CollapseHead::SetText(const base::string16& text) {
  label_->SetText(text);
}

void CollapseHead::Layout() {
  label_->SetBoundsRect(GetContentsBounds());
}

gfx::Size CollapseHead::GetPreferredSize() const {
  return gfx::Size(128, 16);
}

const SkColor CollapseBorderView::kBorderColor = SkColorSetRGB(11, 11, 11);
const SkColor CollapseBorderView::kBorderShadowColor = SkColorSetRGB(56, 56, 56);

// 
const char CollapseView::kViewClassName[] = "lord::CollapseView";
CollapseView::CollapseView() {
  head_ = new CollapseHead(this);
  border_view_ = new CollapseBorderView;
  contents_ = new views::View;
  
  AddChildView(border_view_);
  AddChildView(head_);
  AddChildView(contents_);
}

CollapseView::~CollapseView() {
}

const char* CollapseView::GetClassName() const {
  return kViewClassName;
}

void CollapseView::Layout() {
}

const char* CollapseView::GetClassName() const {
}
}  // namespace lord
