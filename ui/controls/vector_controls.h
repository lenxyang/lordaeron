#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/separator.h"
#include "ui/views/controls/textfield/textfield.h"
#include "nelf/controls/editing_label.h"

namespace azer {
class Vector3;
class Vector4;
class Quaternion;
}

namespace lord {
class Vector3Control : public views::View,
                       public views::TextfieldController {
 public:
  static const char kViewClassName[];
  Vector3Control();
  ~Vector3Control() override;

  azer::Vector3* vector3() { return vector_;}
  void SetVector3(azer::Vector3* vector);

  void SetUnitSize(gfx::Size size);
  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;

 protected:
  void ContentsChanged(views::Textfield* sender,
                       const base::string16& new_contents) override;
  bool HandleKeyEvent(views::Textfield* sender,
                      const ui::KeyEvent& key_event) override;
 private:
  void UpdateControlValue();
  gfx::Size unit_size_;
  azer::Vector3* vector_;
  nelf::EditingLabel* xlabel_, *ylabel_, *zlabel_;
  DISALLOW_COPY_AND_ASSIGN(Vector3Control);
};
}  // namespace lord
