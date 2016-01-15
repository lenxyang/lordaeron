#include "lordaeron/interactive/rotation_controller.h"

#include "base/logging.h"
#include "azer/math/math.h"
#include "azer/render/render.h"
#include "azer/render/geometry.h"
#include "lordaeron/env.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/ui/render_window.h"
#include "lordaeron/interactive/interactive_context.h"

namespace lord {

using namespace azer;

namespace {
bool PickingCircle(const azer::Vector3& pos, float radius,  const azer::Plane& p,
                   const azer::Ray& ray, const Camera& camera, float* depth) {
  if (std::abs(ray.directional().dot(p.normal()) - 1.0) < 0.002) { 
    return true;
  }

  Vector3 pt = p.intersect(ray);
  if (std::abs(pt.distance(pos) - radius) < 0.2) {
    Vector4 v = camera.GetProjViewMatrix() * Vector4(pt, 1.0f);
    *depth = v.z / v.w;
    return true;
  } else {
    *depth = 100.0f;
    return false;
  }
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
  SceneNode* node = context_->GetPickingNode();
  if (node) {
    InitControllerObject(node);
  }
}

void RotationController::OnOperationStop() {
  rotating_axis_ = kAxisNone;
}

void RotationController::OnLostFocus() {
  dragging_ = false;
}

bool RotationController::OnMousePressed(const ui::MouseEvent& event) {
  SceneNode* node = NULL;
  if (rotating_axis_ != kAxisNone) {
    SceneNode* node = context_->GetPickingNode();
    DCHECK(node);
    int32 axis = GetSelectedAxis(event.location());
    if (axis != kAxisNone) {
      dragging_ = true;
      location_ = event.location();
      origin_orient_ = node->orientation();
    }

    return true;
  }

  node = context_->GetObjectFromLocation(event.location());
  context_->SetPickingNode(node);
  if (node) {
    InitControllerObject(node);
    return true;
  } else {
    return false;
  }
}

bool RotationController::OnMouseDragged(const ui::MouseEvent& event) {
  if (dragging_) {
    SceneNode* node = context_->GetPickingNode();
    DCHECK(node);
    float width = context_->window()->GetContentsBounds().width();
    float height = context_->window()->GetContentsBounds().height();
    Quaternion quaternion(origin_orient_);
    node->set_orientation(origin_orient_);
    switch (rotating_axis_) {
      case kAxisX: {
        float offset = (float)(event.location().y() - location_.y()) / height;
        Radians rad(kPI * 4.0f * offset);
        node->rotate(Vector3(1.0f, 0.0f, 0.0f), rad);
        break;
      }
      case kAxisY: {
        float offset = (float)(event.location().x() - location_.x()) / width;
        Radians rad(kPI * 4.0f * offset);
        node->rotate(Vector3(0.0f, 1.0f, 0.0f), rad);
        break;
      }
      case kAxisZ: {
        float offset = (float)(event.location().x() - location_.x()) / width;
        Radians rad(kPI * 4.0f * offset);
        node->rotate(Vector3(0.0f, 0.0f, 1.0f), rad);
        break;
      }
      default:
        dragging_ = false;
        break;
    }
  }
  return true;
}

bool RotationController::OnMouseReleased(const ui::MouseEvent& event) {
  dragging_ = false;
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

void RotationController::InitControllerObject(SceneNode* node) {
  float radius = node->vmin().distance(node->vmax()) * 0.5f;
  Vector3 pos = (node->vmin() + node->vmax()) * 0.5f;
  object_->SetPosition(pos);
  object_->set_radius(radius);
}

int32 RotationController::GetSelectedAxis(gfx::Point location) {
  SceneNode* node = context_->GetPickingNode();
  const Camera& camera = context_->window()->camera();
  DCHECK(node);
  Vector3 pos = (node->vmin() + node->vmax()) * 0.5f;
  float radius = node->vmin().distance(node->vmax()) * 0.5f;
  Ray ray = context_->GetPickingRay(location);
  Plane px(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
  Plane py(Vector3(0.0f, 1.0f, 0.0f), -pos.y);
  Plane pz(Vector3(0.0f, 0.0f, 1.0f), -pos.z);

  float depth1, depth2, depth3;
  bool pick1 = PickingCircle(pos, radius, px, ray, camera, &depth1);
  bool pick2 = PickingCircle(pos, radius, py, ray, camera, &depth2);
  bool pick3 = PickingCircle(pos, radius, pz, ray, camera, &depth3);
  if (depth1 < depth2 && depth1 < depth3 && pick1) {
    return kAxisX;
  } else if (depth2 < depth3 && depth2 < depth1 && pick2) {
    return kAxisY;
  } else if (depth3 < depth1 && depth3 < depth2 && pick3) {
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
  MeshPartPtr part = CreateCircleMeshPart(effect->vertex_desc(), radius_, 128);
  DCHECK(part->entity_count() == 1);
  circle_ = part->entity_at(0);
  reset_color();

  axis_world_[0] = std::move(RotateZ(Degree(90.0f)));
  axis_world_[1] = Matrix4::kIdentity;
  axis_world_[2] = std::move(RotateX(Degree(-90.0f)));

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
  LordEnv* context = LordEnv::instance();
  effect_->SetDirLight(context->GetInternalLight());
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

  GeoSphereParams param;
  param.slice = 24;
  param.stack = 24;
  param.radius = 1.0f;
  MeshPartPtr part = CreateSphereMeshPart(effect_->vertex_desc(), param);
  DCHECK(part->entity_count() == 1);
  sphere_ = part->entity_at(0);
  circles_.reset(new CircleCoordinateObject(effect_));

  rasterizer_state_ = rs->CreateRasterizerState();
  rasterizer_state_->SetCullingMode(kCullNone);
}

RotationControllerObject::~RotationControllerObject() {
}

void RotationControllerObject::set_radius(float r) {
  radius_ = r;
  circles_->set_radius(r * 1.01);
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
  ScopedRasterizerState scoped_culling(renderer, rasterizer_state_);
  Matrix4 world = Translate(position_);
  Matrix4 lworld = std::move(world * Scale(radius_, radius_, radius_));
  renderer->SetPrimitiveTopology(kTriangleList);
  LordEnv* context = LordEnv::instance();

  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  effect_->SetDirLight(context->GetInternalLight());
  effect_->SetColor(sphere_color_);
  effect_->SetWorld(lworld);
  effect_->SetPV(pv);
  renderer->UseEffect(effect_.get());
  sphere_->Render(renderer);
  renderer->ResetBlending();

  // the sphere will forbidden the circle behind other cirle
  circles_->Render(world, pv, renderer);
}
}  // namespace lord
