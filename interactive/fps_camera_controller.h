#pragma once

#include "lordaeron/interactive/interactive_controller.h"

namespace lord {

class InteractiveContext;

class FPSCameraController : public InteractiveController {
 public:
  FPSCameraController();
  ~FPSCameraController() override;

  // override from InteractiveController
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
  void OnOperationStart(InteractiveContext* ctx) override;
  void OnOperationStop() override;
  void OnLostFocus() override;
  bool OnKeyPressed(const ui::KeyEvent& event) override;
  bool OnKeyReleased(const ui::KeyEvent& event) override;
  bool OnMousePressed(const ui::MouseEvent& event) override;
  bool OnMouseDragged(const ui::MouseEvent& event) override;
  bool OnMouseReleased(const ui::MouseEvent& event) override;
 private:
  void ResetState();
  void RotateCamera(const gfx::Point& prev, const gfx::Point& cur);

  azer::Camera* camera_;
  azer::Quaternion origin_orient_;
  bool orientation_dragging_;
  bool posx_, posy_, posz_;
  bool negx_, negy_, negz_;
  gfx::Point location_;
  DISALLOW_COPY_AND_ASSIGN(FPSCameraController);
};
}
