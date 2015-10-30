#pragma once

#include "azer/render/render.h"
#include "nelf/nelf.h"

namespace lord {
class InteractiveContext;
class InteractiveController {
 public:
  InteractiveController();
  virtual ~InteractiveController();

  void Start(InteractiveContext* ctx);
  void Stop();
  virtual void Update(const azer::FrameArgs& args) {}
  virtual void Render(azer::Renderer* renderer) {}
  virtual void OnOperationStart(InteractiveContext* ctx) {}
  virtual void OnOperationStop() {}
  virtual void OnLostFocus() = 0;

  // if return false, context will take over
  virtual bool OnKeyPressed(const ui::KeyEvent& event) { return false;}
  virtual bool OnKeyReleased(const ui::KeyEvent& event) { return false;}
  virtual bool OnMousePressed(const ui::MouseEvent& event) { return false;}
  virtual bool OnMouseDragged(const ui::MouseEvent& event) { return false;}
  virtual bool OnMouseReleased(const ui::MouseEvent& event) { return false;}
 protected:
  InteractiveContext* context_;
  DISALLOW_COPY_AND_ASSIGN(InteractiveController);
};
}  // namespace lord
