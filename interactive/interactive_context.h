#pragma once

#include "base/memory/scoped_ptr.h"
#include "azer/render/render.h"
#include "nelf/render/event_listener.h"

namespace lord {

class SceneNode;
class InteractiveController;

class InteractiveContext : public nelf::EventListener {
 public:
  InteractiveContext(SceneNode* root);
  ~InteractiveContext();
  
  void SetInteractiveController(scoped_ptr<InteractiveController> controller);

  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);

  SceneNode* root() { return root_;}

  // override from nelf::EventListener
  void OnKeyPressed(const ui::KeyEvent& event) override;
  void OnKeyReleased(const ui::KeyEvent& event) override;
  void OnMousePressed(const ui::MouseEvent& event) override;
  void OnMouseDragged(const ui::MouseEvent& event) override;
  void OnMouseReleased(const ui::MouseEvent& event) override;;
  void OnMouseCaptureLost() override;
  void OnLostFocus() override;
 private:
  SceneNode* root_;
  scoped_ptr<InteractiveController> controller_;
  DISALLOW_COPY_AND_ASSIGN(InteractiveContext);
};
}  // namespace lord
