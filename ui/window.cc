#include "lordaeron/ui/window.h"

#include "nelf/theme/nine_pic_nonclient_frame_theme.h"
#include "nelf/theme/five_pic_nonclient_frame_theme.h"

namespace lord {
Window::Window(const gfx::Rect& init_bounds, nelf::Window* window)
    : nelf::Window(init_bounds, window) {
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
  widget->set_frame_type(views::Widget::FRAME_TYPE_FORCE_CUSTOM);
  Theme* theme = context()->theme();
  NonClientFrameThemePtr nonclient_theme = theme->CreateNineWindowTheme(this);
  WindowNonClientFrame* frame = new WindowNonClientFrame(this);
  frame->SetTheme(nonclient_theme);

  gfx::Size minsize = minsize_;
  gfx::Size maxsize = maxsize_;
  if (minsize.IsEmpty())
    minsize = context()->setting().pane_minsize();
  if (maxsize.IsEmpty())
    maxsize = context()->setting().pane_maxsize();

  frame->set_min_size(minsize);
  frame->set_max_size(maxsize);
  return frame;
}
}  // namespace lord
