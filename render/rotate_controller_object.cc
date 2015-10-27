#include "lordaeron/render/rotate_controller_object.h"

#include "azer/math/math.h"
#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/context.h"

namespace lord {
using namespace azer;

CircleCoordinateObject::CircleCoordinateObject(DiffuseEffectPtr effect)
    : effect_(effect) {
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
    effect_->Use(renderer);
    circle_->Render(renderer);
  }
}

// class RotateControllerObject
RotateControllerObject::RotateControllerObject() 
    : selected_color_(Vector4(1.0f, 1.0f, 0.0f, 1.0)) {
  RenderSystem* rs = RenderSystem::Current();
  effect_ = CreateDiffuseEffect();
  sphere_color_ = Vector4(1.0f, 1.0f, 1.0f, 0.4f);
  sphere_ = new SphereObject(effect_->GetVertexDesc(), 24, 24);

  circles_.reset(new CircleCoordinateObject(effect_));
}

RotateControllerObject::~RotateControllerObject() {
}

void RotateControllerObject::SetSelectedColor(const azer::Vector4& color) {
  selected_color_ = color;
}

void RotateControllerObject::Render(const azer::Matrix4& world, 
                                    const azer::Matrix4& pv, 
                                    azer::Renderer* renderer) {
  circles_->Render(world, pv, renderer);
  Context* context = Context::instance();
  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  effect_->SetDirLight(context->GetInternalLight());
  effect_->SetColor(sphere_color_);
  effect_->SetWorld(world);
  effect_->SetPV(pv);
  effect_->Use(renderer);
  sphere_->Render(renderer);
  renderer->ResetBlending();
}
}  // namespace lord
