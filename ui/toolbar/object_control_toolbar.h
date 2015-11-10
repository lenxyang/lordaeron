#pragma once

#include "nelf/nelf.h"

namespace lord {
class InteractiveContext;
class ObjectControlToolbar : public views::ButtonListener {
 public:
  ObjectControlToolbar(nelf::MainFrame* mainframe, InteractiveContext* ctx);
  ~ObjectControlToolbar();

  // override from nelf::ToolbarDelegate
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;
 private:
  nelf::Toolbar* toolbar_;
  InteractiveContext* interactive_;
  DISALLOW_COPY_AND_ASSIGN(ObjectControlToolbar);
};
}  // namespace lord
