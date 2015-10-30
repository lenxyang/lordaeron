#pragma once

#include "nelf/nelf.h"

namespace lord {
class InteractiveContext;
class ObjectControlToolbar : public nelf::ToolbarButtonListener {
 public:
  ObjectControlToolbar(nelf::Mainframe* mainframe, InteractiveContext* ctx);
  ~ObjectControlToolbar();

  // override from nelf::ToolbarDelegate
  void OnToolbarButtonPressed(views::Button* sender) override;
 private:
  nelf::Toolbar* toolbar_;
  scoped_ptr<nelf::SimpleToolbarModel> model_;
  InteractiveContext* interactive_;
  DISALLOW_COPY_AND_ASSIGN(ObjectControlToolbar);
};
}  // namespace lord
