#include "lordaeron/ui/scene_node_control_pane.h"

#include "base/strings/utf_string_conversions.h"
#include "ui/views/view.h"
#include "ui/views/layout/box_layout.h"
#include "nelf/context.h"
#include "lordaeron/env.h"
#include "lordaeron/ui/iconset.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {

using base::UTF8ToUTF16;
SceneNodeControPane::SceneNodeControPane(SceneNode* node)
    : node_(node) {
  using views::BoxLayout;
  using views::Button;
  LordEnv* ctx = LordEnv::instance();
  SetLayoutManager(new BoxLayout(BoxLayout::kHorizontal, 0, 1, 0));

  gfx::ImageSkia ico_unlock = ctx->GetIcon(Iconset::kIconSceneUnlock);
  gfx::ImageSkia ico_lock = ctx->GetIcon(Iconset::kIconSceneLock);
  gfx::ImageSkia ico_visible = ctx->GetIcon(Iconset::kIconSceneVisible);
  gfx::ImageSkia ico_invisible = ctx->GetIcon(Iconset::kIconSceneInvisible);
  gfx::ImageSkia ico_pickable = ctx->GetIcon(Iconset::kIconScenePickable);
  gfx::ImageSkia ico_unpickable = ctx->GetIcon(Iconset::kIconSceneUnpickable);
  gfx::ImageSkia ico_bv = ctx->GetIcon(Iconset::kIconSceneBounding);
  gfx::ImageSkia ico_nbv = ctx->GetIcon(Iconset::kIconSceneNBounding);

  btn_locked_ = new views::ToggleImageButton(this);
  btn_locked_->SetToggledTooltipText(UTF8ToUTF16("locked"));
  btn_locked_->SetImage(Button::STATE_NORMAL, &ico_lock);
  btn_locked_->SetImage(Button::STATE_HOVERED, &ico_lock);
  btn_locked_->SetImage(Button::STATE_PRESSED, &ico_unlock);
  btn_locked_->SetImage(Button::STATE_DISABLED, &ico_lock);
  btn_locked_->SetToggledImage(Button::STATE_NORMAL, &ico_unlock);
  btn_locked_->SetToggledImage(Button::STATE_HOVERED, &ico_unlock);
  btn_locked_->SetToggledImage(Button::STATE_PRESSED, &ico_lock);
  btn_locked_->SetToggledImage(Button::STATE_DISABLED, &ico_unlock);

  btn_visible_ = new views::ToggleImageButton(this);
  btn_visible_->SetToggledTooltipText(UTF8ToUTF16("Visible"));
  btn_visible_->SetImage(Button::STATE_NORMAL, &ico_invisible);
  btn_visible_->SetImage(Button::STATE_HOVERED, &ico_invisible);
  btn_visible_->SetImage(Button::STATE_PRESSED, &ico_visible);
  btn_visible_->SetImage(Button::STATE_DISABLED, &ico_invisible);
  btn_visible_->SetToggledImage(Button::STATE_NORMAL, &ico_visible);
  btn_visible_->SetToggledImage(Button::STATE_HOVERED, &ico_visible);
  btn_visible_->SetToggledImage(Button::STATE_PRESSED, &ico_invisible);
  btn_visible_->SetToggledImage(Button::STATE_DISABLED, &ico_visible);

  btn_pickable_ = new views::ToggleImageButton(this);
  btn_pickable_->SetToggledTooltipText(UTF8ToUTF16("Pickable"));
  btn_pickable_->SetImage(Button::STATE_NORMAL, &ico_unpickable);
  btn_pickable_->SetImage(Button::STATE_HOVERED, &ico_unpickable);
  btn_pickable_->SetImage(Button::STATE_PRESSED, &ico_pickable);
  btn_pickable_->SetImage(Button::STATE_DISABLED, &ico_unpickable);
  btn_pickable_->SetToggledImage(Button::STATE_NORMAL, &ico_pickable);
  btn_pickable_->SetToggledImage(Button::STATE_HOVERED, &ico_pickable);
  btn_pickable_->SetToggledImage(Button::STATE_PRESSED, &ico_unpickable);
  btn_pickable_->SetToggledImage(Button::STATE_DISABLED, &ico_pickable);

  btn_draw_bv_ = new views::ToggleImageButton(this);
  btn_draw_bv_->SetToggledTooltipText(UTF8ToUTF16("Draw Bounding Volumn"));
  btn_draw_bv_->SetImage(Button::STATE_NORMAL,  &ico_nbv);
  btn_draw_bv_->SetImage(Button::STATE_HOVERED,  &ico_nbv);
  btn_draw_bv_->SetImage(Button::STATE_PRESSED, &ico_bv);
  btn_draw_bv_->SetImage(Button::STATE_DISABLED, &ico_nbv);
  btn_draw_bv_->SetToggledImage(Button::STATE_NORMAL,  &ico_bv);
  btn_draw_bv_->SetToggledImage(Button::STATE_HOVERED,  &ico_bv);
  btn_draw_bv_->SetToggledImage(Button::STATE_PRESSED, &ico_nbv);
  btn_draw_bv_->SetToggledImage(Button::STATE_DISABLED, &ico_bv);
  
  AddChildView(btn_locked_);
  AddChildView(btn_visible_);
  AddChildView(btn_pickable_);
  AddChildView(btn_draw_bv_);

  UpdateState(node);
}

SceneNodeControPane::~SceneNodeControPane() {
}

void SceneNodeControPane::UpdateState(SceneNode* node) {
  btn_visible_->SetToggled(node->visible());
  btn_pickable_->SetToggled(node->pickable());
  btn_draw_bv_->SetToggled(node->is_draw_bounding_volumn());
}

void SceneNodeControPane::ButtonPressed(views::Button* sender, 
                                         const ui::Event& event) {
  if (btn_visible_ == sender) {
    node_->set_visible(!node_->visible());
  } else if (btn_pickable_ == sender) {
    node_->set_pickable(!node_->pickable());
  } else if (btn_draw_bv_ == sender) {
    node_->set_draw_bounding_volumn(!node_->is_draw_bounding_volumn());
  }

  UpdateState(node_);
}

}  // namespace lord
