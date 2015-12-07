#pragma once

#include "base/memory/scoped_ptr.h"
#include "azer/render/render.h"
#include "nelf/render/event_listener.h"

namespace lord {

class SceneNode;
class SceneRenderWindow;
class InteractiveController;

class InteractiveContext : public nelf::EventListener {
 public:
  InteractiveContext(SceneRenderWindow* win);
  ~InteractiveContext();
  
  void ResetController();
  void SetController(scoped_ptr<InteractiveController> controller);

  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);

  SceneNode* root();
  SceneRenderWindow* window() { return window_;}

  void SetPickingNode(SceneNode* node);
  SceneNode* GetPickingNode() { return picking_node_;}
  SceneNode* GetObjectFromLocation(const gfx::Point& point);
  azer::Ray GetPickingRay(const gfx::Point& pt);
  azer::Vector4 CalcWorldPosFromScreen(const gfx::Point& pt);

  // override from nelf::EventListener
  void OnKeyPressed(const ui::KeyEvent& event) override;
  void OnKeyReleased(const ui::KeyEvent& event) override;
  void OnMousePressed(const ui::MouseEvent& event) override;
  void OnMouseDragged(const ui::MouseEvent& event) override;
  void OnMouseReleased(const ui::MouseEvent& event) override;;
  void OnMouseMoved(const ui::MouseEvent& event) override;
  void OnMouseCaptureLost() override;
  void OnLostFocus() override;
 private:
  SceneRenderWindow* window_;
  SceneNode* picking_node_;
  scoped_ptr<InteractiveController> controller_;
  DISALLOW_COPY_AND_ASSIGN(InteractiveContext);
};
}  // namespace lord
