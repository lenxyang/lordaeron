#pragma once

#include "ui/views/controls/button/image_button.h"
#include "nelf/nelf.h"

namespace lord {
class SceneNode;

class SceneNodeControPanel : public views::View, 
                             public views::ButtonListener {
 public:
  SceneNodeControPanel(SceneNode* node);
  ~SceneNodeControPanel() override;

  // override from views::ButtonListener
  void ButtonPressed(views::Button* sender, const ui::Event& event) override;
 private:
  void UpdateState(SceneNode* node);
  SceneNode* node_;
  views::ToggleImageButton* btn_visible_;
  views::ToggleImageButton* btn_pickable_;
  views::ToggleImageButton* btn_draw_bv_;
  views::ToggleImageButton* btn_locked_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeControPanel);
};
}  // namespace lord
