#include "lordaeron/interactive/scale_controller.h"

namespace lord {
ScaleController::ScaleController() {
}

ScaleController::~ScaleController() {
}

  // override
void ScaleController::Update(const azer::FrameArgs& args) {
}

void ScaleController::Render(azer::Renderer* renderer) {
}

void ScaleController::OnOperationStart(InteractiveContext* ctx) {
}

void ScaleController::OnOperationStop() {
}

void ScaleController::OnLostFocus() {
}

bool ScaleController::OnMousePressed(const ui::MouseEvent& event) {
  return true;
}

bool ScaleController::OnMouseDragged(const ui::MouseEvent& event) {
  return true;
}

bool ScaleController::OnMouseReleased(const ui::MouseEvent& event) {
  return true;
}

void ScaleController::OnMouseMoved(const ui::MouseEvent& event) {
}

// class ScaleAxisPlaneObject
ScaleAxisPlaneObject::ScaleAxisPlaneObject() {
}

ScaleAxisPlaneObject::~ScaleAxisPlaneObject() {
}

void ScaleAxisPlaneObject::InitPlane() {
}
}  // namespace lord
