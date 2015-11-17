#include "lordaeron/ui/controls/vector_controls.h"

#include "base/strings/string16.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/views/border.h"
#include "ui/views/layout/box_layout.h"

#include "azer/math/vector3.h"
#include "azer/math/vector4.h"

namespace lord {
using base::UTF8ToUTF16;
using base::StringPrintf;

const char Vector3Control::kViewClassName[] = "lord::Vector3Control";
Vector3Control::Vector3Control() 
    : vector_(NULL),
      editing_(NULL) {
  using views::Border;
  xlabel_ = new views::Label();
  ylabel_ = new views::Label();
  zlabel_ = new views::Label();
  AddChildView(xlabel_);
  AddChildView(ylabel_);
  AddChildView(zlabel_);

  unit_size_ = gfx::Size(72, 24);
  SkColor color = SkColorSetRGB(0, 0, 0);
  SetBorder(Border::CreateSolidSidedBorder(1, 1, 1, 1, color));
  xlabel_->SetBorder(Border::CreateSolidSidedBorder(0, 0, 0, 1, color));
  ylabel_->SetBorder(Border::CreateSolidSidedBorder(0, 0, 0, 1, color));
}

Vector3Control::~Vector3Control() {
}

void Vector3Control::SetVector3(azer::Vector3* vector) { 
  vector_ = vector;
  if (vector_)
    UpdateControlValue();
}

void Vector3Control::UpdateControlValue() {
  if (vector_) {
    xlabel_->SetText(UTF8ToUTF16(StringPrintf("X: %.02f", vector_->x)));
    ylabel_->SetText(UTF8ToUTF16(StringPrintf("Y: %.02f", vector_->y)));
    zlabel_->SetText(UTF8ToUTF16(StringPrintf("Z: %.02f", vector_->z)));
  }
}

const char* Vector3Control::GetClassName() const {
  return kViewClassName;
}

gfx::Size Vector3Control::GetPreferredSize() const {
  gfx::Size size = unit_size_;
  size.set_width(size.width() * 3);
  return size;
}

void Vector3Control::SetUnitSize(gfx::Size size) {
  if (unit_size_ != size) {
    unit_size_ = size;
    Layout();
  }
}

void Vector3Control::Layout() {
  int32 x = GetContentsBounds().x();
  int32 y = GetContentsBounds().y();
  gfx::Size size = unit_size_;
  int32 width = size.width();
  xlabel_->SetBoundsRect(gfx::Rect(gfx::Point(x, y), size));
  ylabel_->SetBoundsRect(gfx::Rect(gfx::Point(x + width, y), size));
  zlabel_->SetBoundsRect(gfx::Rect(gfx::Point(x + width * 2, y), size));
}

bool Vector3Control::CanProcessEventsWithinSubtree() const {
  return true;
}

void Vector3Control::LayoutEditor(views::Textfield* editor) {
  DCHECK(editing_);
  editor->SetBoundsRect(editing_->bounds());
  AddChildView(editor);
}

void Vector3Control::OnEditBegin(views::Textfield* editor) {
}

void Vector3Control::OnEditEnd(views::Textfield* editor, bool commited) {
  editing_ = NULL;
}

bool Vector3Control::OnMousePressed(const ui::MouseEvent& event) {
  DCHECK(editing_ == NULL);
  gfx::Size size = unit_size_;
  if (event.location().x() > 0 && event.location().x() < size.width()) {
    editing_ = xlabel_;
  } else if (event.location().x() < size.width() * 2) {
    editing_ = ylabel_;
  } else if (event.location().x() < size.width() * 3) {
    editing_ = zlabel_;
  }

  if (editing_) {
    editing_helper_.reset(new nelf::EditingHelper(editing_));
    editing_helper_->StartEditing(this);
  }

  return false;
}
}  // namespace lord
