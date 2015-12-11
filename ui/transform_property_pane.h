#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "nelf/nelf.h"
#include "lordaeron/ui/controls/vector_control.h"
#include "azer/render/render.h"

namespace lord {
class SceneNode;
class TransformContents : public views::View,
                          public VectorControlDelegate {
 public:
  static const char kViewClassName[];
  explicit TransformContents(SceneNode* node);
  ~TransformContents() override;

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  void OnVectorChanged(VectorControl* control) override;
  SceneNode* node_;
  views::Textfield* namefield_;
  views::Label* position_label_;
  views::Label* orientation_label_;
  views::Label* scale_label_;
  VectorControl* position_control_;
  VectorControl* orientation_control_;
  VectorControl* scale_control_;
  DISALLOW_COPY_AND_ASSIGN(TransformContents);
};

class TransformPropertyPane : public nelf::CollapseView {
 public:
  static const char kViewClassName[];
  explicit TransformPropertyPane(SceneNode* node);
  const char* GetClassName() const override;
  void Layout() override;
 private:
  TransformContents* contents_;
  DISALLOW_COPY_AND_ASSIGN(TransformPropertyPane);
};
}  // namespace lord
