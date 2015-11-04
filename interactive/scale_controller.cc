#include "lordaeron/interactive/scale_controller.h"

#include "base/logging.h"
#include "lordaeron/context.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/interactive/controller_object.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/util/picking.h"

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
ScaleAxisPlaneObject::ScaleAxisPlaneObject(DiffuseEffect* effect)
    : effect_(effect),
      axis_length_(1.0f),
      inner_(0.35),
      outer_(0.55) {

  color_[0] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);  // xy
  color_[1] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);  // yz
  color_[2] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);  // zy
}

ScaleAxisPlaneObject::~ScaleAxisPlaneObject() {
}

void ScaleAxisPlaneObject::set_length(float length) {
  axis_length_ = length;
  InitPlane();
  InitPlaneFrame();
}


void ScaleAxisPlaneObject::InitPlane() {
  Vector4 pos[4] = {
    Vector4(0.0f, outer_ * axis_length_, 0.0f, 1.0f),
    Vector4(outer_ * axis_length_, 0.0f, 0.0f, 1.0f),
    Vector4(inner_ * axis_length_, 0.0f, 0.0f, 1.0f),
    Vector4(0.0f, inner_ * axis_length_, 0.0f, 1.0f),
  };
  plane_ = lord::CreatePlane(pos, effect_->GetVertexDesc());
}

void ScaleAxisPlaneObject::InitPlaneFrame() {
  Vector4 pos[4] = {
    Vector4(0.0f, outer_ * axis_length_, 0.0f, 1.0f),
    Vector4(outer_ * axis_length_, 0.0f, 0.0f, 1.0f),
    Vector4(inner_ * axis_length_, 0.0f, 0.0f, 1.0f),
    Vector4(0.0f, inner_ * axis_length_, 0.0f, 1.0f),
  };
  plane_frame_ = CreateLineList(pos, (int32)arraysize(pos), effect_->GetVertexDesc());
}
}  // namespace lord
