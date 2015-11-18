#include "lordaeron/ui/directional_light_pane.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "lordaeron/ui/controls/color_controls.h"
#include "lordaeron/ui/controls/vector_controls.h"

namespace lord {
const int32 DirectionalLightPane::kVerticalMargin = 10;;
const int32 DirectionalLightPane::kHorzMargin = 10;
const char DirectionalLightPane::kViewClassName[] = "nelf::DirectionalLightPane";
DirectionalLightPane::DirectionalLightPane() {
  dir_label_ = new views::Label;
  dir_label_->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
  dir_label_->SetText(::base::UTF8ToUTF16("Directional:"));
  dir_control_ = new Vector3Control;
  AddChildView(dir_control_);
  AddChildView(dir_label_);

  ambient_label_ = new views::Label;
  ambient_label_->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
  ambient_label_->SetText(::base::UTF8ToUTF16("Ambient:"));
  ambient_control_ = new ColorControl;
  AddChildView(ambient_control_);
  AddChildView(ambient_label_);

  diffuse_label_ = new views::Label;
  diffuse_label_->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
  diffuse_label_->SetText(::base::UTF8ToUTF16("Diffuse:"));
  diffuse_control_ = new ColorControl;
  AddChildView(diffuse_control_);
  AddChildView(diffuse_label_);

  specular_label_ = new views::Label;
  specular_label_->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
  specular_label_->SetText(::base::UTF8ToUTF16("Specular:"));
  specular_control_ = new ColorControl;
  AddChildView(specular_control_);
  AddChildView(specular_label_);
}

DirectionalLightPane::~DirectionalLightPane() {
}

void DirectionalLightPane::SetControlLight(Light* light) {
  DCHECK_EQ(light->type(), kDirectionalLight);
  light_ = light;
  DirLight* l = light_->mutable_dir_light();
  dir_control_->SetVector3(&l->direction);
  ambient_control_->SetColor(&l->ambient);
  diffuse_control_->SetColor(&l->diffuse);
  specular_control_->SetColor(&l->specular);
}

const char* DirectionalLightPane::GetClassName() const {
  return kViewClassName;
}

gfx::Size DirectionalLightPane::GetPreferredSize() const {
  return gfx::Size(320, 240);
}

void DirectionalLightPane::Layout() {
  int32 kMidline = 80;

  int y = 18;
  dir_control_->SizeToPreferredSize();
  dir_control_->SetPosition(gfx::Point(kMidline + kHorzMargin, y));
  gfx::Size dir_label_size = dir_label_->GetPreferredSize();
  dir_label_size.set_height(dir_control_->height());
  dir_label_->SetBoundsRect(gfx::Rect(
      gfx::Point(kMidline - dir_label_size.width(), y), dir_label_size));

  y += dir_label_size.height();
  y += kVerticalMargin;

  ambient_control_->SizeToPreferredSize();
  ambient_control_->SetPosition(gfx::Point(kMidline + kHorzMargin, y));
  gfx::Size ambient_label_size = ambient_label_->GetPreferredSize();
  ambient_label_size.set_height(ambient_control_->height());
  ambient_label_->SetBoundsRect(gfx::Rect(
      gfx::Point(kMidline - ambient_label_size.width(), y), ambient_label_size));

  y += ambient_label_size.height();
  y += kVerticalMargin;

  diffuse_control_->SizeToPreferredSize();
  diffuse_control_->SetPosition(gfx::Point(kMidline + kHorzMargin, y));
  gfx::Size diffuse_label_size = diffuse_label_->GetPreferredSize();
  diffuse_label_size.set_height(diffuse_control_->height());
  diffuse_label_->SetBoundsRect(gfx::Rect(
      gfx::Point(kMidline - diffuse_label_size.width(), y), diffuse_label_size));

  y += diffuse_label_size.height();
  y += kVerticalMargin;

  specular_control_->SizeToPreferredSize();
  specular_control_->SetPosition(gfx::Point(kMidline + kHorzMargin, y));
  gfx::Size specular_label_size = specular_label_->GetPreferredSize();
  specular_label_size.set_height(specular_control_->height());
  specular_label_->SetBoundsRect(gfx::Rect(
      gfx::Point(kMidline - specular_label_size.width(), y), specular_label_size));
}

}  // namespace lord
