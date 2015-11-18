#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "lordaeron/render/light.h"

namespace lord {
class ColorControl;
class Vector3Control;
class DirectionalLightPane : public views::View {
 public:
  static const char kViewClassName[];
  DirectionalLightPane();
  ~DirectionalLightPane() override;

  void SetControlLight(Light* light);

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  views::Textfield* diffuse;
  LightPtr light_;
  Vector3Control* dir_control_;
  ColorControl* ambient_control_;
  ColorControl* diffuse_control_;
  ColorControl* specular_control_;
  views::Label* dir_label_;
  views::Label* ambient_label_;
  views::Label* diffuse_label_;
  views::Label* specular_label_;
  static const int32 kVerticalMargin;
  static const int32 kHorzMargin;
  DISALLOW_COPY_AND_ASSIGN(DirectionalLightPane);
};
}  // namespace lord
