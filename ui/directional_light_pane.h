#pragma once

#include "ui/views/view.h"
#include "lordaeron/render/light.h"

namespace nelf {
class DirectionalLightPane : public views::View {
 public:
  static const char kViewClassName[];
  DirectionalLightPane();

  void SetControlLight(Light* light);

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  Light* light_;
  DISALLOW_COPY_AND_ASSIGN(DirectionalLightPane);
};
}  // namespace nelf
