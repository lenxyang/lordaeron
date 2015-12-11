#pragma once

#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "azer/render/render.h"

namespace lord {
class VectorControl;
class VectorControlDelegate {
 public:
  virtual void OnVectorChanged(VectorControl* control) = 0;
};

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
  explicit VectorControl(const azer::Vector2& vec);
  explicit VectorControl(const azer::Vector3& vec);
  explicit VectorControl(const azer::Vector4& vec);
  explicit VectorControl(const azer::Quaternion& q);

  int type() const { return type_;}
  void set_readonly(bool readonly);
  void readonly() const;
  const azer::Vector2& GetVec2Value() const { return vec2_;}
  const azer::Vector3& GetVec3Value() const { return vec3_;}
  const azer::Vector4& GetVec4Value() const { return vec4_;}
  const azer::Quaternion& GetQuaternionValue() const { return quaternion_;}

  void set_delegate(VectorControlDelegate* delegate);
  VectorControlDelegate* delegate() { return delegate_;}

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
  azer::Vector2 vec2_;
  azer::Vector3 vec3_;
  azer::Vector4 vec4_;
  azer::Quaternion quaternion_;
  VectorControlDelegate* delegate_;
  int32 type_;
  DISALLOW_COPY_AND_ASSIGN(VectorControl);
};
}  // namespace lord
