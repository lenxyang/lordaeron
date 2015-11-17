#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/separator.h"
#include "ui/views/controls/textfield/textfield.h"
#include "nelf/controls/editing_helper.h"

namespace azer {
class Vector3;
class Vector4;
class Quaternion;
}

namespace lord {
class Vector3Control : public views::View,
                       public nelf::EditingHelperDelegate {
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
  bool CanProcessEventsWithinSubtree() const override;
  bool OnMousePressed(const ui::MouseEvent& event) override;

  // override from EditingHelperDelegate
  void LayoutEditor(views::Textfield* editor) override;
  void OnEditBegin(views::Textfield* editor) override;
  void OnEditEnd(views::Textfield* editor, bool commited) override;
 private:
  void UpdateControlValue();
  gfx::Size unit_size_;
  azer::Vector3* vector_;
  views::Label* xlabel_, *ylabel_, *zlabel_;
  views::Label* editing_;
  scoped_ptr<nelf::EditingHelper> editing_helper_;
  DISALLOW_COPY_AND_ASSIGN(Vector3Control);
};
}  // namespace lord
