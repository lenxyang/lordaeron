#include "lordaeron/interactive/scale_controller.h"

#include "base/logging.h"
#include "lordaeron/context.h"
#include "lordaeron/interactive/controller_object.h"
#include "lordaeron/interactive/scale_controller.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/util/picking.h"

namespace lord {
using namespace azer;
ScaleController::ScaleController() {
  object_.reset(new ScaleControllerObject);
}

ScaleController::~ScaleController() {
}

  // override
void ScaleController::Update(const azer::FrameArgs& args) {
}

void ScaleController::Render(azer::Renderer* renderer) {
  SceneNode* node = context_->GetPickingNode();
  if (node) {
    UpdateControllerObjectPos();
  }
}

void ScaleController::OnOperationStart(InteractiveContext* ctx) {
}

void ScaleController::OnOperationStop() {
}

void ScaleController::OnLostFocus() {
  object_->reset_selected();
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

void ScaleController::UpdateControllerObjectPos() {
  SceneNode* node = context_->GetPickingNode();
  DCHECK(node);
  Vector3 pos = (node->vmin() + node->vmax()) * 0.5f;
  float radius = node->vmin().distance(node->vmax()) * 0.5f;
  object_->set_length(radius * kAxisLengthMultiply);
  object_->reset_selected();
  object_->SetPosition(pos);
}

// class ScaleAxisPlaneObject
ScaleAxisPlaneObject::ScaleAxisPlaneObject(DiffuseEffect* effect)
    : effect_(effect),
      axis_length_(1.0f),
      inner_(0.35f),
      outer_(0.55f) {

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


// class ScaleControllerObject
ScaleControllerObject::ScaleControllerObject() {
  axis_.reset(new XYZAxisObject);
  plane_.reset(new ScaleAxisPlaneObject);
}

ScaleControllerObject::~ScaleControllerObject() {
}

void ScaleControllerObject::reset_selected() {
  memset(selected_axis_, 0, sizeof(selected_axis_));
  memset(selected_plane_, 0, sizeof(selected_plane_));
}

void ScaleControllerObject::set_selected_axis(int32 axis) {
  DCHECK(axis < 3);
  selected_axis_[axis] = 1;
}

void ScaleControllerObject::set_selected_plane(int32 plane) {
  DCHECK(plane < 3);
  selected_plane_[axis] = 1;
}

void ScaleControllerObject::SetPV(const azer::Matrix4& pv) {
  axis_->SetPV(pv);
  plane_->SetPosition(pos);
}

void ScaleControllerObject::SetPosition(const azer::Vector3& pos) {
  axis_->SetPosition(pos);
  plane_->SetPosition(pos);
}

void ScaleControllerObject::Render(azer::Renderer* renderer) {
  axis_->Render(renderer);
  plane_->Render(renderer);
}
}  // namespace lord
