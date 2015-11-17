#include "lordaeron/ui/controls/vector_controls.h"

#include "ui/views/layout/box_layout.h"

namespace lord {
const char Vecotor3Control::kViewClassName[] = "lord::Vecotor3Control";
Vecotor3Control::Vecotor3Control() {
}

Vecotor3Control::~Vecotor3Control() {
}

const char* Vecotor3Control::GetClassName() const {
  return kViewClassName;
}

gfx::Size Vecotor3Control::GetPreferredSize() const {
}

void Vecotor3Control::Layout() {
}
}  // namespace lord
