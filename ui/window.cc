#include "lordaeron/ui/window.h"

#include "ui/views/layout/box_layout.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/button/image_button.h"
#include "nelf/nelf.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"
#include "lordaeron/ui/iconset.h"

namespace lord {
Window::Window(const gfx::Rect& init_bounds, nelf::Window* window)
    : nelf::Window(init_bounds, window) {
  // int32 id = IDR_ICON_CAPTION_NEXT_ARROW;
  int32 id = IDR_ICON_CAPTION_TRIANGLE_CLEAR_RIGHT;
  Context* ctx = Context::instance();
  nelf::ResourceBundle* bundle = ctx->resource_bundle();
  const gfx::ImageSkia* img = bundle->GetImageSkiaNamed(id);
  SetWindowIcon(*img);
  SetShowIcon(true);
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

void Window::ButtonPressed(views::Button* sender, const ui::Event& event) {
  if (sender == close_button_) {
    GetWidget()->Close();
  }
}

views::NonClientFrameView* Window::CreateNonClientFrameView(views::Widget* widget) {
  Context* ctx = Context::instance();
  views::NonClientFrameView* nonclient = nelf::Window::CreateNonClientFrameView(widget);
  DCHECK(nonclient_);
  using nelf::WindowSystemButtonPane;
  WindowSystemButtonPane* system_pane = new WindowSystemButtonPane(nonclient_);
  nelf::ToggleButton* btn = new nelf::ToggleButton(
      ctx->GetIcon(Iconset::kIconWindowClose));
  btn->SetMinSize(gfx::Size(20, 20));
  btn->SetMaxSize(gfx::Size(20, 20));
  btn->set_listener(this);
  system_pane->AddChildView(btn);
  system_pane->SizeToPreferredSize();
  nonclient_->SetSystemButtonPane(system_pane);
  return nonclient;
}
}  // namespace lord
