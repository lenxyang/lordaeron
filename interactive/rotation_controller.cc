#include "lordaeron/interactive/rotation_controller.h"

#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/interactive_context.h"

namespace lord {

using namespace azer;
RotationController::RotationController() {
}
RotationController::~RotationController() {
}

void RotationController::Update(const azer::FrameArgs& args) {
}

void RotationController::Render(azer::Renderer* renderer) {
}

void RotationController::OnLostFocus() {
}

bool RotationController::OnMousePressed(const ui::MouseEvent& event) {
  return false;
}

bool RotationController::OnMouseDragged(const ui::MouseEvent& event) {
  return false;
}

bool RotationController::OnMouseReleased(const ui::MouseEvent& event) {
  return false;
}

void RotationController::OnMouseMoved(const ui::MouseEvent& event) {
}

int32 RotationController::GetSelectedAxis(gfx::Point location) {
  Ray ray = context_->GetPickingRay(location);
  return 0;
}

// class CircleCoordinateObject
CircleCoordinateObject::CircleCoordinateObject(DiffuseEffect* effect)
    :radius_(1.0f),
     effect_(effect) {
  set_radius(1.0f);
  
  circle_ = new CircleObject(effect_->GetVertexDesc(), 64);
  reset_color();

  axis_world_[0] = std::move(RotateY(Degree(-90.0f)));
  axis_world_[1] = std::move(RotateX(Degree(90.0f)));
  axis_world_[2] = Matrix4::kIdentity;
}

CircleCoordinateObject::~CircleCoordinateObject() {
}

void CircleCoordinateObject::reset_color() {
  axis_color_[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
  axis_color_[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
  axis_color_[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
}

void CircleCoordinateObject::set_radius(float radius) {
  radius_ = radius;
  scale_ = Scale(radius, radius, radius);
}

void CircleCoordinateObject::Render(const azer::Matrix4& world,
                                    const azer::Matrix4& pv, 
                                    azer::Renderer* renderer) {
  Context* context = Context::instance();
  effect_->SetDirLight(context->GetInternalLight());
  for (int i = 0; i < 3; ++i) {
    Matrix4 w = std::move(axis_world_[i] * scale_);
    effect_->SetColor(axis_color_[i]);
    effect_->SetWorld(world * w);
    effect_->SetPV(pv);
    renderer->UseEffect(effect_.get());
    circle_->Render(renderer);
  }
}

// class RotationControllerObject
RotationControllerObject::RotationControllerObject() 
    : selected_axis_(RotationController::kAxisNone),
      selected_color_(Vector4(1.0f, 1.0f, 0.0f, 1.0)) {
  RenderSystem* rs = RenderSystem::Current();
  effect_ = CreateDiffuseEffect();
  sphere_color_ = Vector4(1.0f, 1.0f, 1.0f, 0.4f);
  sphere_ = new SphereObject(effect_->GetVertexDesc(), 24, 24);

  circles_.reset(new CircleCoordinateObject(effect_));
}

RotationControllerObject::~RotationControllerObject() {
}

void RotationControllerObject::set_radius(float r) {
  radius_ = r;
  circles_->set_radius(r);
}

void RotationControllerObject::SetPosition(azer::Vector3& position) {
  position_ = position;
}

void RotationControllerObject::SetSelectedColor(const azer::Vector4& color) {
  selected_color_ = color;
}

void RotationControllerObject::SetSelectedAxis(int32 axis) {
  circles_->reset_color();
  switch (axis) {
    case RotationController::kAxisX:
      circles_->set_xaxis_color(selected_color_);
      break;
    case RotationController::kAxisY:
      circles_->set_yaxis_color(selected_color_);
      break;
    case RotationController::kAxisZ:
      circles_->set_zaxis_color(selected_color_);
      break;
    default:
      break;
  }
}

void RotationControllerObject::Render(const azer::Matrix4& pv, 
                                      azer::Renderer* renderer) {
  Matrix4 world = Translate(position_);
  circles_->Render(world, pv, renderer);
  Context* context = Context::instance();
  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  effect_->SetDirLight(context->GetInternalLight());
  effect_->SetColor(sphere_color_);
  effect_->SetWorld(world);
  effect_->SetPV(pv);
  renderer->UseEffect(effect_.get());
  sphere_->Render(renderer);
  renderer->ResetBlending();
}
}  // namespace lord
