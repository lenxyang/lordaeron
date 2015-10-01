#pragma once

#include "nelf/nelf.h"

namespace lord {
class ObjectControlToolbar : public nelf::ToolbarButtonListener {
 public:
  ObjectControlToolbar(nelf::Mainframe* mainframe);
  ~ObjectControlToolbar();

  // override from nelf::ToolbarDelegate
  void OnToolbarButtonPressed(int id) override;
 private:
  nelf::Toolbar* toolbar_;
  DISALLOW_COPY_AND_ASSIGN(ObjectControlToolbar);
};
}  // namespace lord
