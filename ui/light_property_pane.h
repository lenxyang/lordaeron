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

class SpotLightAttenuationPane : public views::View {
 public:
  static const char kViewClassName[];
  explicit SpotLightAttenuationPane(Light* light);
  ~SpotLightAttenuationPane() override;

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  views::Label* range_label_;
  views::Label* theta_label_;
  views::Label* phi_label_;
  views::Label* falloff_label_;
  views::Textfield* range_textfield_;
  views::Textfield* theta_textfield_;
  views::Textfield* phi_textfield_;
  views::Textfield* falloff_textfield_;
  nelf::GroupView* color_group_;
  LightPtr light_;
  DISALLOW_COPY_AND_ASSIGN(SpotLightAttenuationPane);
};

class SpotLightContents : public views::View {
 public:
  static const char kViewClassName[];
  explicit SpotLightContents(Light* light);
  ~SpotLightContents() override;

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  void LayoutColorControls();
  LightPtr light_;
  LightColorPane* pane_;
  SpotLightAttenuationPane* attenuation_pane_;
  DISALLOW_COPY_AND_ASSIGN(SpotLightContents);
};

class SpotLightPane : public nelf::CollapseView {
 public:
  static const char kViewClassName[];
  explicit SpotLightPane(Light* light);
  const char* GetClassName() const override;
  void Layout() override;
 private:
  SpotLightContents* contents_;
  DISALLOW_COPY_AND_ASSIGN(SpotLightPane);
};

class PointLightAttenuationPane : public views::View {
 public:
  static const char kViewClassName[];
  explicit PointLightAttenuationPane(Light* light);
  ~PointLightAttenuationPane() override;

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  views::Label* range_label_;
  views::Label* const_label_;
  views::Label* linear_label_;
  views::Label* quadratic_label_;
  views::Textfield* range_textfield_;
  views::Textfield* const_textfield_;
  views::Textfield* linear_textfield_;
  views::Textfield* quadratic_textfield_;
  nelf::GroupView* color_group_;
  LightPtr light_;
  DISALLOW_COPY_AND_ASSIGN(PointLightAttenuationPane);
};

class PointLightContents : public views::View {
 public:
  static const char kViewClassName[];
  explicit PointLightContents(Light* light);
  ~PointLightContents() override;

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  void LayoutColorControls();
  LightPtr light_;
  LightColorPane* pane_;
  PointLightAttenuationPane* attenuation_pane_;
  DISALLOW_COPY_AND_ASSIGN(PointLightContents);
};

class PointLightPane : public nelf::CollapseView {
 public:
  static const char kViewClassName[];
  explicit PointLightPane(Light* light);
  const char* GetClassName() const override;
  void Layout() override;
 private:
  PointLightContents* contents_;
  DISALLOW_COPY_AND_ASSIGN(PointLightPane);
};
}  // namespace lord
