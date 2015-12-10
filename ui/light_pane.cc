#include "lordaeron/ui/light_pane.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/views/border.h"
#include "lordaeron/ui/color_util.h"

namespace lord {

using base::UTF8ToUTF16;
using views::Border;
const char LightColorPane::kViewClassName[] = "nelf::LightColorPane";
LightColorPane::LightColorPane(Light* light) 
    : light_(light) {
  color_group_ = new nelf::GroupView(UTF8ToUTF16("Color"));
  AddChildView(color_group_);
  ambient_label_ = new views::Label;
  ambient_label_->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
  ambient_label_->SetText(::base::UTF8ToUTF16("Ambient:"));
  ambient_control_ = new nelf::ColorButton;
  ambient_control_->set_delegate(this);
  color_group_->contents()->AddChildView(ambient_control_);
  color_group_->contents()->AddChildView(ambient_label_);

  diffuse_label_ = new views::Label;
  diffuse_label_->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
  diffuse_label_->SetText(::base::UTF8ToUTF16("Diffuse:"));
  diffuse_control_ = new nelf::ColorButton;
  diffuse_control_->set_delegate(this);
  color_group_->contents()->AddChildView(diffuse_control_);
  color_group_->contents()->AddChildView(diffuse_label_);

  specular_label_ = new views::Label;
  specular_label_->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
  specular_label_->SetText(::base::UTF8ToUTF16("Specular:"));
  specular_control_ = new nelf::ColorButton;
  specular_control_->set_delegate(this);
  color_group_->contents()->AddChildView(specular_control_);
  color_group_->contents()->AddChildView(specular_label_);

  SetLight(light);
}

const char* LightColorPane::GetClassName() const {
  return kViewClassName;
}

gfx::Size LightColorPane::GetPreferredSize() const {
  return gfx::Size(220, 130);
}

void LightColorPane::Layout() {
  color_group_->SetBoundsRect(std::move(GetContentsBounds()));
  int32 kMidline = 70;
  const int32 kVerticalMargin = 10;;
  const int32 kHorzMargin = 10, kLineHeight = 28;
  const int32 kColorButtonWidth = 70, kColorButtonHeight = 22;
  float y = 10;
  gfx::Rect group_bounds = std::move(GetContentsBounds());
  group_bounds.set_height(130);
  group_bounds.set_width(170);
  color_group_->SetBoundsRect(group_bounds);

  ambient_control_->SetPosition(gfx::Point(kMidline + kHorzMargin, y));
  ambient_control_->SetSize(gfx::Size(kColorButtonWidth, kColorButtonHeight));
  gfx::Size ambient_label_size = ambient_label_->GetPreferredSize();
  ambient_label_size.set_height(kColorButtonHeight);
  ambient_label_->SetBoundsRect(gfx::Rect(
      gfx::Point(kMidline - ambient_label_size.width(), y), ambient_label_size));

  y += ambient_label_size.height();
  y += kVerticalMargin;

  diffuse_control_->SetPosition(gfx::Point(kMidline + kHorzMargin, y));
  diffuse_control_->SetSize(gfx::Size(kColorButtonWidth, kColorButtonHeight));
  gfx::Size diffuse_label_size = diffuse_label_->GetPreferredSize();
  diffuse_label_size.set_height(kColorButtonHeight);
  diffuse_label_->SetBoundsRect(gfx::Rect(
      gfx::Point(kMidline - diffuse_label_size.width(), y), diffuse_label_size));

  y += diffuse_label_size.height();
  y += kVerticalMargin;

  specular_control_->SetPosition(gfx::Point(kMidline + kHorzMargin, y));
  specular_control_->SetSize(gfx::Size(kColorButtonWidth, kColorButtonHeight));
  gfx::Size specular_label_size = specular_label_->GetPreferredSize();
  specular_label_size.set_height(kColorButtonHeight);
  specular_label_->SetBoundsRect(gfx::Rect(
      gfx::Point(kMidline - specular_label_size.width(), y), specular_label_size));
}

void LightColorPane::SetLight(Light* light) {
  light_ = light;
  // dir_control_->SetVector3(&l->direction);
  ambient_control_->SetColor(SkColorFromVector4(light->ambient()));
  diffuse_control_->SetColor(SkColorFromVector4(light->diffuse()));
  specular_control_->SetColor(SkColorFromVector4(light->specular()));
}

void LightColorPane::OnColorChanged(nelf::ColorButton* color_btn,
                                              SkColor color) {
  if (color_btn == ambient_control_) {
    light_->set_ambient(Vector4FromSkColor(color));
  } else if (color_btn == diffuse_control_) {
    light_->set_diffuse(Vector4FromSkColor(color));
  } else if (color_btn == specular_control_) {
    light_->set_specular(Vector4FromSkColor(color));
  } else {
    NOTREACHED();
  }
}

// class DirectionalLightContents
const char DirectionalLightContents::kViewClassName[] = "nelf::DirectionalLightContents";
DirectionalLightContents::DirectionalLightContents(Light* light) 
    : light_(light) {
  pane_ = new LightColorPane(light);
  AddChildView(pane_);
  pane_->SizeToPreferredSize();
}

DirectionalLightContents::~DirectionalLightContents() {}

const char* DirectionalLightContents::GetClassName() const {
  return kViewClassName;
}

gfx::Size DirectionalLightContents::GetPreferredSize() const {
  return gfx::Size(320, 240);
}

void DirectionalLightContents::Layout() {
  gfx::Rect color_pane_bounds = GetContentsBounds();
  pane_->SetPosition(color_pane_bounds.origin());
}

// 
const char DirectionalLightPane::kViewClassName[] = "nelf::DirectionalLightPane";
DirectionalLightPane::DirectionalLightPane(Light* light)
    : nelf::CollapseView(::base::UTF8ToUTF16("Directional Light")) {
  contents_ = new DirectionalLightContents(light);
  GetContents()->AddChildView(contents_);
  GetContents()->SetLayoutManager(new views::FillLayout);
  Expand();
}

const char* DirectionalLightPane::GetClassName() const {
  return kViewClassName;
}

void DirectionalLightPane::Layout() {
  gfx::Size size = std::move(GetPreferredSize());
  if (parent()) {
    size.set_width(parent()->GetContentsBounds().width());
  }
  SetCollapseSize(size);
  nelf::CollapseView::Layout();
}

// class SpotLightContents
const char SpotLightContents::kViewClassName[] = "nelf::SpotLightContents";
SpotLightContents::SpotLightContents(Light* light) 
    : light_(light) {
  pane_ = new LightColorPane(light);
  AddChildView(pane_);
  pane_->SizeToPreferredSize();
}

SpotLightContents::~SpotLightContents() {}

const char* SpotLightContents::GetClassName() const {
  return kViewClassName;
}

gfx::Size SpotLightContents::GetPreferredSize() const {
  return gfx::Size(320, 240);
}

void SpotLightContents::Layout() {
  gfx::Rect color_pane_bounds = GetContentsBounds();
  pane_->SetPosition(color_pane_bounds.origin());
}

// 
const char SpotLightPane::kViewClassName[] = "nelf::SpotLightPane";
SpotLightPane::SpotLightPane(Light* light)
    : nelf::CollapseView(::base::UTF8ToUTF16("Spot Light")) {
  contents_ = new SpotLightContents(light);
  GetContents()->AddChildView(contents_);
  GetContents()->SetLayoutManager(new views::FillLayout);
  Expand();
}

const char* SpotLightPane::GetClassName() const {
  return kViewClassName;
}

void SpotLightPane::Layout() {
  gfx::Size size = std::move(GetPreferredSize());
  if (parent()) {
    size.set_width(parent()->GetContentsBounds().width());
  }
  SetCollapseSize(size);
  nelf::CollapseView::Layout();
}

// class PointLightContents
const char PointLightContents::kViewClassName[] = "nelf::PointLightContents";
PointLightContents::PointLightContents(Light* light) 
    : light_(light) {
  pane_ = new LightColorPane(light);
  AddChildView(pane_);
  pane_->SizeToPreferredSize();
}

PointLightContents::~PointLightContents() {}

const char* PointLightContents::GetClassName() const {
  return kViewClassName;
}

gfx::Size PointLightContents::GetPreferredSize() const {
  return gfx::Size(320, 240);
}

void PointLightContents::Layout() {
  gfx::Rect color_pane_bounds = GetContentsBounds();
  pane_->SetPosition(color_pane_bounds.origin());
}

// 
const char PointLightPane::kViewClassName[] = "nelf::PointLightPane";
PointLightPane::PointLightPane(Light* light)
    : nelf::CollapseView(::base::UTF8ToUTF16("Point Light")) {
  contents_ = new PointLightContents(light);
  GetContents()->AddChildView(contents_);
  GetContents()->SetLayoutManager(new views::FillLayout);
  Expand();
}

const char* PointLightPane::GetClassName() const {
  return kViewClassName;
}

void PointLightPane::Layout() {
  gfx::Size size = std::move(GetPreferredSize());
  if (parent()) {
    size.set_width(parent()->GetContentsBounds().width());
  }
  SetCollapseSize(size);
  nelf::CollapseView::Layout();
}
}  // namespace lord
