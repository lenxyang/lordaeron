#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/separator.h"
#include "ui/views/controls/textfield/textfield.h"
#include "nelf/controls/editing_label.h"

namespace azer {
class Vector3;
class Vector4;
class Quaternion;
}

namespace lord {
class ColorControl : public views::View,
                       public views::TextfieldController {
 public:
  static const char kViewClassName[];
  ColorControl();
  ~ColorControl() override;

  azer::Vector4* color() { return color_;}
  void SetColor(azer::Vector4* vector);

  void SetUnitSize(gfx::Size size);
  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;

 protected:
  void ContentsChanged(views::Textfield* sender,
                       const base::string16& new_contents) override;
  bool HandleKeyEvent(views::Textfield* sender,
                      const ui::KeyEvent& key_event) override;
 private:
  void UpdateControlValue();
  gfx::Size unit_size_;
  azer::Vector4* color_;
  nelf::EditingLabel* rlabel_, *glabel_, *blabel_, *alabel_;
  DISALLOW_COPY_AND_ASSIGN(ColorControl);
};
}  // namespace lord
