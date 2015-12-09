#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "nelf/nelf.h"
#include "lordaeron/effect/light.h"

namespace lord {
class LightColorPane : public views::View,
                       public nelf::ColorButtonDelegate {
 public:
  static const char kViewClassName[];
  explicit LightColorPane(Light* light);
  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
  // overriden from nelf::ColorButtonDelegate
  void OnColorChanged(nelf::ColorButton* color_btn, SkColor color) override;
 private:
  void SetLight(Light* light);
  nelf::GroupView* color_group_;
  nelf::ColorButton* ambient_control_;
  nelf::ColorButton* diffuse_control_;
  nelf::ColorButton* specular_control_;
  views::Label* ambient_label_;
  views::Label* diffuse_label_;
  views::Label* specular_label_;
  LightPtr light_;
  DISALLOW_COPY_AND_ASSIGN(LightColorPane);
};

class DirectionalLightContents : public views::View {
 public:
  static const char kViewClassName[];
  explicit DirectionalLightContents(Light* light);
  ~DirectionalLightContents() override;

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  void LayoutColorControls();
  LightPtr light_;
  LightColorPane* pane_;
  DISALLOW_COPY_AND_ASSIGN(DirectionalLightContents);
};

class DirectionalLightPane : public nelf::CollapseView {
 public:
  static const char kViewClassName[];
  explicit DirectionalLightPane(Light* light);
  const char* GetClassName() const override;
  void Layout() override;
 private:
  DirectionalLightContents* contents_;
  DISALLOW_COPY_AND_ASSIGN(DirectionalLightPane);
};
}  // namespace lord
