#include "lordaeron/ui/directional_light_pane.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "lordaeron/ui/controls/vector_controls.h"

namespace lord {
const char DirectionalLightPane::kViewClassName[] = "nelf::DirectionalLightPane";
DirectionalLightPane::DirectionalLightPane() {
  dir_label_ = new views::Label;
  dir_label_->SetHorizontalAlignment(gfx::ALIGN_RIGHT);
  dir_label_->SetText(::base::UTF8ToUTF16("Directional:"));
  dir_control_ = new Vector3Control;
  AddChildView(dir_control_);
  AddChildView(dir_label_);
}

DirectionalLightPane::~DirectionalLightPane() {
}

void DirectionalLightPane::SetControlLight(Light* light) {
  DCHECK_EQ(light->type(), kDirectionalLight);
  light_ = light;
  DirLight* l = light_->mutable_dir_light();
  dir_control_->SetVector3(&l->direction);
}

const char* DirectionalLightPane::GetClassName() const {
  return kViewClassName;
}

gfx::Size DirectionalLightPane::GetPreferredSize() const {
  return gfx::Size(400, 300);
}

void DirectionalLightPane::Layout() {
  int32 midline = 100;
  dir_control_->SizeToPreferredSize();
  dir_control_->SetPosition(gfx::Point(110, 10));
  gfx::Size dir_label_size = dir_label_->GetPreferredSize();
  dir_label_size.set_height(dir_control_->height());
  dir_label_->SetBoundsRect(gfx::Rect(
      gfx::Point(midline - dir_label_size.width(), 10), dir_label_size));
}
}  // namespace lord
