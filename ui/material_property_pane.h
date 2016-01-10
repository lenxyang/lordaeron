#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "nelf/nelf.h"
#include "lordaeron/effect/light.h"

namespace lord {
class MaterialPropertyPane : public views::View {
 public:
  MaterialPropertyPane();
  ~MaterialPropertyPane();
 private:
  DISALLOW_COPY_AND_ASSIGN(MaterialPropertyPane);
};
}  // namespace lord
