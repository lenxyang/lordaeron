#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "nelf/nelf.h"
#include "lordaeron/effect/light.h"

namespace lord {
class DirectionalLightPane : public views::View,
                             public nelf::ColorButtonDelegate {
 public:
  static const char kViewClassName[];
  DirectionalLightPane();
  ~DirectionalLightPane() override;

  void SetControlLight(Light* light);

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
  // overriden from nelf::ColorButtonDelegate
  void OnColorChanged(ColorButton* color_btn, SkColor color) override;
 private:
  LightPtr light_;
  nelf::ColorButton* ambient_control_;
  nelf::ColorButton* diffuse_control_;
  nelf::ColorButton* specular_control_;
  views::Label* dir_label_;
  views::Label* ambient_label_;
  views::Label* diffuse_label_;
  views::Label* specular_label_;
  static const int32 kLineHeight;
  static const int32 kVerticalMargin;
  static const int32 kHorzMargin;
  DISALLOW_COPY_AND_ASSIGN(DirectionalLightPane);
};
}  // namespace lord
