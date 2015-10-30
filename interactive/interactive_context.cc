#include "lordaeron/interactive/interactive_context.h"

#include "lordaeron/interactive/interactive_controller.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
InteractiveContext::InteractiveContext(SceneNode* root) 
    : root_(root) {
}

InteractiveContext::~InteractiveContext() {
  if (controller_.get())
    controller_->OnOperationStop();
}

void InteractiveContext::Update(const azer::FrameArgs& args) {
  if (controller_.get())
    controller_->Update(args);
}

void InteractiveContext::Render(azer::Renderer* renderer) {
  if (controller_.get())
    controller_->Render(renderer);
}

void InteractiveContext::SetInteractiveController(
    scoped_ptr<InteractiveController> controller) {
  if (controller_.get())
    controller_->OnOperationStop();

  controller_ = controller.Pass();
  controller_->OnOperationStart(this);
}

void InteractiveContext::OnKeyPressed(const ui::KeyEvent& event) {
  if (controller_.get()) {
    controller_->OnKeyPressed(event);
  }
}

void InteractiveContext::OnKeyReleased(const ui::KeyEvent& event) {
  if (controller_.get()) {
    controller_->OnKeyReleased(event);
  }
}

void InteractiveContext::OnMousePressed(const ui::MouseEvent& event) {
  if (controller_.get()) {
    controller_->OnMousePressed(event);
  }
}

void InteractiveContext::OnMouseDragged(const ui::MouseEvent& event) {
  if (controller_.get()) {
    controller_->OnMouseDragged(event);
  }
}

void InteractiveContext::OnMouseReleased(const ui::MouseEvent& event) {
  if (controller_.get()) {
    controller_->OnMouseReleased(event);
  }
}

void InteractiveContext::OnMouseCaptureLost() {
  if (controller_.get()) {
    controller_->OnLostFocus();
  }
}

void InteractiveContext::OnLostFocus() {
  if (controller_.get()) {
    controller_->OnLostFocus();
  }
}
}  // namespace lord
