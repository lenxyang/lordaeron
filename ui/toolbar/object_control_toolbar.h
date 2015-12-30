#pragma once

#include "nelf/nelf.h"

namespace lord {
class InteractiveContext;
class ObjectControlToolbar : public nelf::Toolbar,
                             nelf::GroupButtonPaneDelegate {
 public:
  ObjectControlToolbar(nelf::MainFrame* mainframe, InteractiveContext* ctx);
  ~ObjectControlToolbar();

  // override from nelf::ToolbarDelegate
  void OnToggleButtonStateChanged(nelf::ToggleButton* btn) override;
 private:
  InteractiveContext* interactive_;
  DISALLOW_COPY_AND_ASSIGN(ObjectControlToolbar);
};
}  // namespace lord
