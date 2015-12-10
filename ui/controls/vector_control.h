#pragma once

#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "azer/render/render.h"

namespace lord {
class VectorControl : public views::View,
                      public views::TextfieldController {
 public:
  static const char kViewClassName[];
  enum {
    kTypeVector2,
    kTypeVector3,
    kTypeVector4,
    kTypeQuaternion,
  };
  explicit VectorControl(azer::Vector2* vec);
  explicit VectorControl(azer::Vector3* vec);
  explicit VectorControl(azer::Vector4* vec);
  explicit VectorControl(azer::Quaternion* q);

  void UpdateUIFromData();
  void UpdateDataFromUI();

  // override from views
  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
  
  // override from TextfieldController;
  void ContentsChanged(views::Textfield* sender,
                       const base::string16& new_contents) override;
 private:
  void InitUI();
  base::string16 label_text(int i) const;
  views::Label *label_[4];
  views::Textfield *textfield_[4];
  azer::Vector2* vec2_;
  azer::Vector3* vec3_;
  azer::Vector4* vec4_;
  azer::Quaternion* quaternion_;
  int32 type_;
  DISALLOW_COPY_AND_ASSIGN(VectorControl);
};
}  // namespace lord
