#pragma once

#include "lordaeron/interactive/interactive_controller.h"

namespace lord {

class InteractiveContext;
class SceneNode;

class PickingController : public InteractiveController {
 public:
  PickingController();
  ~PickingController() override;

  void OnOperationStart(InteractiveContext* ctx) override;
  bool OnKeyPressed(const ui::KeyEvent& event) override;

  void SetPickingNode(SceneNode* node) { picking_node_ = node;}
  SceneNode* GetPickingNode() { return picking_node_;}
 private:
  InteractiveContext* context_;
  SceneNode* picking_node_;
  DISALLOW_COPY_AND_ASSIGN(PickingController);
};
}
