#include "lordaeron/ui/directional_light_pane.h"

namespace lord {
const char DirectionalLightPane::kViewClassName[] = "nelf::DirectionalLightPane";
DirectionalLightPane::DirectionalLightPane() {
}

DirectionalLightPane::~DirectionalLightPane() {
}

void DirectionalLightPane::SetControlLight(Light* light) {
  light_ = light;
}

const char* DirectionalLightPane::GetClassName() const {
  return kViewClassName;
}

gfx::Size DirectionalLightPane::GetPreferredSize() const {
  return gfx::Size(400, 300);
}

void DirectionalLightPane::Layout() {
}
}  // namespace lord
