#pragma once

#include "lordaeron/interactive/interactive_controller.h"

namespace lord {

class InteractiveContext;
class SceneNode;

class ScaleController : public InteractiveController {
 public:
  ScaleController();
  ~ScaleController() override;

  // override
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
  void OnLostFocus() override;
  bool OnMousePressed(const ui::MouseEvent& event);
 private:
  azer::MeshPtr mesh_;
  DISALLOW_COPY_AND_ASSIGN(ScaleController);
};
}
