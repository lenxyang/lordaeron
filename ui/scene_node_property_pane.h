#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "nelf/nelf.h"
#include "lordaeron/ui/controls/vector_control.h"
#include "azer/render/render.h"

namespace lord {
class SceneNode;
class SceneNodeContents : public views::View {
 public:
  static const char kViewClassName[];
  explicit SceneNodeContents(SceneNode* node);
  ~SceneNodeContents() override;

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
  void ChildPreferredSizeChanged(views::View* child) override;
  void Layout() override;
 private:
  SceneNode* node_;
  views::Label* name_label_;
  views::Textfield* name_textfield_;
  views::Label* vmin_label_;
  views::Label* vmax_label_;
  VectorControl* vmin_control_;
  VectorControl* vmax_control_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeContents);
};

class SceneNodePropertyPane : public nelf::CollapseView {
 public:
  static const char kViewClassName[];
  explicit SceneNodePropertyPane(SceneNode* node);
  const char* GetClassName() const override;
  void Layout() override;
 private:
  SceneNodeContents* contents_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodePropertyPane);
};
}  // namespace lord
