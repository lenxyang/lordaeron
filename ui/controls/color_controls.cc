#include "lordaeron/ui/controls/color_controls.h"

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

const char ColorControl::kViewClassName[] = "lord::ColorControl";
ColorControl::ColorControl() 
    : color_(NULL) {
  using views::Border;
  rlabel_ = new nelf::EditingLabel();
  glabel_ = new nelf::EditingLabel();
  blabel_ = new nelf::EditingLabel();
  alabel_ = new nelf::EditingLabel();
  rlabel_->SetPattern(UTF8ToUTF16("R: %s"));
  glabel_->SetPattern(UTF8ToUTF16("G: %s"));
  blabel_->SetPattern(UTF8ToUTF16("B: %s"));
  alabel_->SetPattern(UTF8ToUTF16("A: %s"));
  rlabel_->SetTextfieldController(this);
  glabel_->SetTextfieldController(this);
  blabel_->SetTextfieldController(this);
  alabel_->SetTextfieldController(this);
  AddChildView(rlabel_);
  AddChildView(glabel_);
  AddChildView(blabel_);
  AddChildView(alabel_);

  unit_size_ = gfx::Size(48, 24);
  SkColor color = SkColorSetRGB(0, 0, 0);
  SetBorder(Border::CreateSolidSidedBorder(1, 1, 1, 1, color));
  rlabel_->SetBorder(Border::CreateSolidSidedBorder(0, 0, 0, 1, color));
  glabel_->SetBorder(Border::CreateSolidSidedBorder(0, 0, 0, 1, color));
  blabel_->SetBorder(Border::CreateSolidSidedBorder(0, 0, 0, 1, color));
  SetFocusable(true);
}

ColorControl::~ColorControl() {
}

void ColorControl::SetColor(azer::Vector4* color) { 
  color_ = color;
  if (color_)
    UpdateControlValue();
}

void ColorControl::UpdateControlValue() {
  if (color_) {
    rlabel_->SetContents(UTF8ToUTF16(StringPrintf("%.02f", color_->x)));
    glabel_->SetContents(UTF8ToUTF16(StringPrintf("%.02f", color_->y)));
    blabel_->SetContents(UTF8ToUTF16(StringPrintf("%.02f", color_->z)));
    alabel_->SetContents(UTF8ToUTF16(StringPrintf("%.02f", color_->w)));
  }
}

const char* ColorControl::GetClassName() const {
  return kViewClassName;
}

gfx::Size ColorControl::GetPreferredSize() const {
  gfx::Size size = unit_size_;
  size.set_width(size.width() * 4);
  return size;
}

void ColorControl::SetUnitSize(gfx::Size size) {
  if (unit_size_ != size) {
    unit_size_ = size;
    Layout();
  }
}

void ColorControl::Layout() {
  int32 x = GetContentsBounds().x();
  int32 y = GetContentsBounds().y();
  gfx::Size size = unit_size_;
  int32 width = size.width();
  rlabel_->SetBoundsRect(gfx::Rect(gfx::Point(x, y), size));
  glabel_->SetBoundsRect(gfx::Rect(gfx::Point(x + width, y), size));
  blabel_->SetBoundsRect(gfx::Rect(gfx::Point(x + width * 2, y), size));
  alabel_->SetBoundsRect(gfx::Rect(gfx::Point(x + width * 3, y), size));
}

bool ColorControl::HandleKeyEvent(views::Textfield* sender,
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

void ColorControl::ContentsChanged(views::Textfield* sender,
                                     const base::string16& text) {
  double d;
  bool ret = ::base::StringToDouble(::base::UTF16ToUTF8(text), &d);
  if (!ret) {
    UpdateControlValue();
    return;
  }
  
  if (rlabel_ == sender->parent()) {
    color_->x = d;
  } else if (glabel_ == sender->parent()) {
    color_->y = d;
  } else if (blabel_ == sender->parent()) {
    color_->z = d;
  } else if (alabel_ == sender->parent()) {
    color_->w = d;
  } else {
    NOTREACHED();
  }
}
}  // namespace lord
