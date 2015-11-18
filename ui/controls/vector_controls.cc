#include "lordaeron/ui/controls/vector_controls.h"

#include "base/strings/string16.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/strings/string_number_conversions.h"
#include "ui/views/border.h"
#include "ui/views/layout/box_layout.h"

#include "azer/math/vector3.h"
#include "azer/math/vector4.h"

namespace lord {
using base::UTF8ToUTF16;
using base::StringPrintf;

const char Vector3Control::kViewClassName[] = "lord::Vector3Control";
Vector3Control::Vector3Control() 
    : vector_(NULL) {
  using views::Border;
  xlabel_ = new nelf::EditingLabel();
  ylabel_ = new nelf::EditingLabel();
  zlabel_ = new nelf::EditingLabel();
  xlabel_->SetPattern(UTF8ToUTF16("X: %s"));
  ylabel_->SetPattern(UTF8ToUTF16("Y: %s"));
  zlabel_->SetPattern(UTF8ToUTF16("Z: %s"));
  xlabel_->SetTextfieldController(this);
  ylabel_->SetTextfieldController(this);
  zlabel_->SetTextfieldController(this);
  AddChildView(xlabel_);
  AddChildView(ylabel_);
  AddChildView(zlabel_);

  unit_size_ = gfx::Size(72, 24);
  SkColor color = SkColorSetRGB(0, 0, 0);
  SetBorder(Border::CreateSolidSidedBorder(1, 1, 1, 1, color));
  xlabel_->SetBorder(Border::CreateSolidSidedBorder(0, 0, 0, 1, color));
  ylabel_->SetBorder(Border::CreateSolidSidedBorder(0, 0, 0, 1, color));
  SetFocusable(true);
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
    xlabel_->SetContents(UTF8ToUTF16(StringPrintf("%.02f", vector_->x)));
    ylabel_->SetContents(UTF8ToUTF16(StringPrintf("%.02f", vector_->y)));
    zlabel_->SetContents(UTF8ToUTF16(StringPrintf("%.02f", vector_->z)));
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

bool Vector3Control::HandleKeyEvent(views::Textfield* sender,
                                    const ui::KeyEvent& key_event) {
  switch (key_event.key_code()) {
    case ui::VKEY_0:
    case ui::VKEY_1:
    case ui::VKEY_2:
    case ui::VKEY_3:
    case ui::VKEY_4:
    case ui::VKEY_5:
    case ui::VKEY_6:
    case ui::VKEY_7:
    case ui::VKEY_8:
    case ui::VKEY_9:
    case ui::VKEY_BACK:
    case ui::VKEY_SHIFT:
    case ui::VKEY_DECIMAL:
    case ui::VKEY_END:
    case ui::VKEY_HOME:
    case ui::VKEY_LEFT:
    case ui::VKEY_UP:
    case ui::VKEY_RIGHT:
    case ui::VKEY_DOWN:
    case ui::VKEY_INSERT:
    case ui::VKEY_DELETE:
      return false;
    default:
      return true;
  }
}

void Vector3Control::ContentsChanged(views::Textfield* sender,
                                     const base::string16& text) {
  double d;
  bool ret = ::base::StringToDouble(::base::UTF16ToUTF8(text), &d);
  if (!ret) {
    UpdateControlValue();
    return;
  }
  
  if (xlabel_ == sender->parent()) {
    vector_->x = d;
  } else if (ylabel_ == sender->parent()) {
    vector_->y = d;
  } else if (zlabel_ == sender->parent()) {
    vector_->z = d;
  } else {
    NOTREACHED();
  }
}
}  // namespace lord
