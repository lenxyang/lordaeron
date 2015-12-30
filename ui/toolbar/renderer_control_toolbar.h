#pragma once

#include "ui/views/controls/button/button.h"
#include "nelf/nelf.h"

namespace lord {
class InteractiveContext;
class RendererControlToolbar : public nelf::Toolbar,
                               views::ButtonListener {
 public:
  RendererControlToolbar(nelf::MainFrame* mainframe);
  ~RendererControlToolbar();
 private:
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;
  DISALLOW_COPY_AND_ASSIGN(RendererControlToolbar);
};
}  // namespace lord
