#pragma once

#include "ui/views/controls/button/button.h"
#include "nelf/nelf.h"

namespace lord {
class InteractiveContext;
class RendererControlToolbar : public views::ButtonListener {
 public:
  RendererControlToolbar(nelf::MainFrame* mainframe);
  ~RendererControlToolbar();

  nelf::Toolbar* toolbar() { return toolbar_;}
 private:
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;
  nelf::Toolbar* toolbar_;
  DISALLOW_COPY_AND_ASSIGN(RendererControlToolbar);
};
}  // namespace lord
