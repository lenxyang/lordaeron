#pragma once

#include "ui/gfx/geometry/rect.h"
#include "nelf/nelf.h"

namespace lord {
class Window : public nelf::Window,
               public views::ButtonListener {
 public:
  Window(const gfx::Rect& init_bounds, nelf::Window* window);
 protected:
  // override from views::View
  void Layout() override;
  // override form views::ButtonListener
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;

  // override from Window
  views::NonClientFrameView* CreateNonClientFrameView(views::Widget* widget) override;
  void OnBeforeWidgetInit(views::Widget::InitParams* params,
                          views::Widget* widget) override;
  void OnAfterWidgetInit() override;

  nelf::CaptionTitle* caption_title_;
  nelf::CaptionIcon* caption_icon_;
  nelf::CaptionBar* captionbar_;
  views::Button* close_button_;
  DISALLOW_COPY_AND_ASSIGN(Window);
};
}  // namespace lord
