#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"

namespace lord {
class Vecotor3Control : public views::View {
 public:
  static const char kViewClassName[];
  Vecotor3Control();
  ~Vecotor3Control() override;

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  DISALLOW_COPY_AND_ASSIGN(Vecotor3Control);
};
}  // namespace lord
