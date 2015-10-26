#include "lordaeron/ui/window.h"

#include "ui/views/layout/box_layout.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/button/image_button.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"

namespace lord {
Window::Window(const gfx::Rect& init_bounds, nelf::Window* window)
    : nelf::Window(init_bounds, window) {
  // int32 id = IDR_ICON_CAPTION_NEXT_ARROW;
  int32 id = IDR_ICON_CAPTION_TRIANGLE_CLEAR_RIGHT;
  Context* ctx = Context::instance();
  nelf::ResourceBundle* bundle = ctx->resource_bundle();
  const gfx::ImageSkia* img = bundle->GetImageSkiaNamed(id);
  set_window_icon(*img);
  set_show_icon(true);
}

void Window::Layout() {
  nelf::Window::Layout();
}

void Window::OnBeforeWidgetInit(views::Widget::InitParams* params,
                                views::Widget* widget) {
  params->type = views::Widget::InitParams::TYPE_WINDOW;
  params->opacity = views::Widget::InitParams::INFER_OPACITY;
  params->keep_on_top = false;
}

void Window::OnAfterWidgetInit() {
}

views::NonClientFrameView* Window::CreateNonClientFrameView(views::Widget* widget) {
  return nelf::Window::CreateNonClientFrameView(widget);
}
}  // namespace lord
