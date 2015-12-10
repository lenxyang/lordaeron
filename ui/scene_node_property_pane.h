#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "nelf/nelf.h"
#include "lordaeron/ui/controls/vector_control.h"
#include "azer/render/render.h"

namespace lord {

class SceneNodePropertyContents : public views::View,
                                  public VectorControlDelegate {
 public:
  static const char kViewClassName[];
  explicit SceneNodePropertyContents(SceneNode* node);
  ~SceneNodePropertyContents() override;

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void Layout() override;
 private:
  void OnVectorChanged(VectorControl* control) override;
  SceneNode* node_;
  views::Label* namelabel_;
  views::Textfield* namefield_;
  VectorControl* position_control_;
  VectorControl* orientation_control_;
  VectorControl* scale_control_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodePropertyContents);
};

class SceneNodePropertyPane : public nelf::CollapseView {
 public:
  static const char kViewClassName[];
  explicit SceneNodePropertyPane(SceneNode* node);
  const char* GetClassName() const override;
  void Layout() override;
 private:
  SceneNodePropertyContents* contents_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodePropertyPane);
};
}  // namespace lord
