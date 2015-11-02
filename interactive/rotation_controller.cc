#include "lordaeron/interactive/rotation_controller.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/interactive/interactive_context.h"

namespace lord {

using namespace azer;

namespace {
bool PickingCircle(const azer::Vector3& pos, float radius,  const azer::Plane& p,
                   const azer::Ray& ray) {
  if (ray.directional() == p.normal())
    return true;

  Vector3 pt = p.intersect(ray);
  if (std::abs(pt.distance(pos) - radius) < 0.1) {
    return true;
  }

  return false;
}
}

RotationController::RotationController() 
    : rotating_axis_(kAxisNone) {
  object_.reset(new RotationControllerObject);
}
RotationController::~RotationController() {
}

void RotationController::Update(const azer::FrameArgs& args) {
}

void RotationController::Render(azer::Renderer* renderer) {
  SceneNode* node = context_->GetPickingNode();
  if (node) {
    const Camera& camera = context_->window()->camera();
    const Matrix4& pv = camera.GetProjViewMatrix();
    object_->Render(pv, renderer);
  }
}

void RotationController::OnOperationStart(InteractiveContext* ctx) {
}

void RotationController::OnOperationStop() {
  rotating_axis_ = kAxisNone;
}

void RotationController::OnLostFocus() {
}

bool RotationController::OnMousePressed(const ui::MouseEvent& event) {
  SceneNode* node = NULL;
  if (rotating_axis_ != kAxisNone) {
    SceneNode* node = context_->GetPickingNode();
    DCHECK(node);
    int32 axis = GetSelectedAxis(event.location());
    return true;
  }

  node = context_->GetObjectFromLocation(event.location());
  context_->SetPickingNode(node);
  if (node) {
    float radius = node->vmin().distance(node->vmax()) * 0.5f;
    Vector3 pos = (node->vmin() + node->vmax()) * 0.5f;
    object_->SetPosition(pos);
    object_->set_radius(radius);
  }

  return true;
}

bool RotationController::OnMouseDragged(const ui::MouseEvent& event) {
  return false;
}

bool RotationController::OnMouseReleased(const ui::MouseEvent& event) {
  return false;
}

void RotationController::OnMouseMoved(const ui::MouseEvent& event) {
  if (context_->GetPickingNode()) {
    int32 axis = GetSelectedAxis(event.location());
    object_->SetSelectedAxis(axis);
    rotating_axis_ = axis;
  } else {
    rotating_axis_ = kAxisNone;
  }
}

int32 RotationController::GetSelectedAxis(gfx::Point location) {
  SceneNode* node = context_->GetPickingNode();
  DCHECK(node);
  Vector3 pos = (node->vmin() + node->vmax()) * 0.5f;
  float radius = node->vmin().distance(node->vmax()) * 0.5f;
  Ray ray = context_->GetPickingRay(location);
  Plane px(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
  Plane py(Vector3(0.0f, 1.0f, 0.0f), -pos.y);
  Plane pz(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
  if (PickingCircle(pos, radius, px, ray)) {
    return kAxisX;
  } else if (PickingCircle(pos, radius, py, ray)) {
    return kAxisY;
  } else if (PickingCircle(pos, radius, pz, ray)) {
    return kAxisZ;
  } else {
    return kAxisNone;
  }
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

  world_[0] = axis_world_[0];
  world_[1] = axis_world_[1];
  world_[2] = axis_world_[2];
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

  world_[0] = std::move(axis_world_[0] * scale_);
  world_[1] = std::move(axis_world_[1] * scale_);
  world_[2] = std::move(axis_world_[2] * scale_);
}

void CircleCoordinateObject::Render(const azer::Matrix4& world,
                                    const azer::Matrix4& pv, 
                                    azer::Renderer* renderer) {
  Context* context = Context::instance();
  effect_->SetDirLight(context->GetInternalLight());
  renderer->SetPrimitiveTopology(kLineList);
  for (int i = 0; i < 3; ++i) {
    Matrix4 w = std::move(world * world_[i]);
    effect_->SetColor(axis_color_[i]);
    effect_->SetWorld(w);
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
  circles_->set_radius(r + 0.001);
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
  Matrix4 lworld = std::move(world * Scale(radius_, radius_, radius_));

  renderer->SetPrimitiveTopology(kTriangleList);
  Context* context = Context::instance();
  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  effect_->SetDirLight(context->GetInternalLight());
  effect_->SetColor(sphere_color_);
  effect_->SetWorld(lworld);
  effect_->SetPV(pv);
  renderer->UseEffect(effect_.get());
  sphere_->Render(renderer);
  renderer->ResetBlending();

  circles_->Render(world, pv, renderer);
}
}  // namespace lord
