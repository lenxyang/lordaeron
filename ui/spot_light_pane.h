#pragma once

#include "ui/views/view.h"
#include "lordaeron/render/light.h"

namespace nelf {
class SpotLightPane : public views::View {
 public:
  static const char kViewClassName[];
  SpotLightPane();

  void SetControlLight(Light* light);

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  Light* light_;
  DISALLOW_COPY_AND_ASSIGN(SpotLightPane);
};
}  // namespace nelf
