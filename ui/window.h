#pragma once

#include "ui/gfx/geometry/rect.h"
#include "nelf/nelf.h"

namespace lord {
class Window : public nelf::Window {
 public:
  Window(const gfx::Rect& init_bounds, nelf::Window* window);
 protected:
  views::NonClientFrameView* CreateNonClientFrameView(views::Widget* widget) override;
  void OnBeforeWidgetInit(views::Widget::InitParams* params,
                          views::Widget* widget) override;
  void OnAfterWidgetInit() override;
  DISALLOW_COPY_AND_ASSIGN(Window);
};
}  // namespace lord
