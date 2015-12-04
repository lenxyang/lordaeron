#pragma once

#include "lordaeron/interactive/interactive_controller.h"

namespace lord {

class InteractiveContext;
class SceneNode;

class PickingObject {
 public:
  virtual void Update(const azer::FrameArgs& args) = 0;
  virtual void Render(azer::Renderer* renderer) = 0;
};

class BoundingVolumnPickingObject : public PickingObject {
 public:
  BoundingVolumnPickingObject(SceneNode* node);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  azer::MeshPtr mesh_;
};

class PickingController : public InteractiveController {
 public:
  PickingController();
  ~PickingController() override;

  // override
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
  void OnLostFocus() override;
  bool OnMousePressed(const ui::MouseEvent& event);
 private:
  scoped_ptr<PickingObject> object_;
  DISALLOW_COPY_AND_ASSIGN(PickingController);
};

}
