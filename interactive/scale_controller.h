#pragma once

#include "lordaeron/interactive/interactive_controller.h"

namespace lord {

class InteractiveContext;
class SceneNode;
class ScaleControllerObject;

class ScaleController : public InteractiveController {
 public:
  ScaleController();
  ~ScaleController() override;

  // override
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
  void OnOperationStart(InteractiveContext* ctx) override;
  void OnOperationStop() override;
  void OnLostFocus() override;
  bool OnMousePressed(const ui::MouseEvent& event) override;
  bool OnMouseDragged(const ui::MouseEvent& event) override;
  bool OnMouseReleased(const ui::MouseEvent& event) override;
  void OnMouseMoved(const ui::MouseEvent& event) override;

  void UpdateControllerObjectPos();
  void UpdateControllerObjectState(const gfx::Point& pt);
 private:
  scoped_ptr<ScaleControllerObject> object_;
  azer::Vector3 origin_scale_;
  bool dragging_;
  DISALLOW_COPY_AND_ASSIGN(ScaleController);
};
}  // namespace lord
