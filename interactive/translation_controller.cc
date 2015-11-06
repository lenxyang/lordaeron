#include "lordaeron/interactive/translation_controller.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/axis_object.h"
#include "lordaeron/interactive/constants.h"
#include "lordaeron/interactive/controller_object.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/util/picking.h"

namespace lord {

// class TransformAxisObject
namespace {
const float kPlaneLength = 0.20f;
const float kAxisLengthMultiply = 1.15f;
}

using namespace azer;
TranslationController::TranslationController() 
    : picking_object_(kPickNone),
      dragging_(false) {
  object_.reset(new TranslationControllerObject);
  object_->set_length(3.0f);
}

TranslationController::~TranslationController() {
}

void TranslationController::Update(const FrameArgs& args) {
}

void TranslationController::Render(Renderer* renderer) {
  SceneNode* node = context_->GetPickingNode();
  if (node) {
    const Camera& camera = context_->window()->camera();
    const Matrix4& pv = camera.GetProjViewMatrix();
    object_->Render(pv, renderer);
  }
}

void TranslationController::OnOperationStart(InteractiveContext* ctx) {
  SceneNode* node = context_->GetPickingNode();
  if (node) {
    UpdateControllerObjectPos();
  }
}

void TranslationController::OnOperationStop() {
}

void TranslationController::UpdateControllerObjectState(
    const gfx::Point& location) {
  SceneNode* node = context_->GetPickingNode();
  DCHECK(node);
  Ray ray = context_->GetPickingRay(location);
  picking_object_ = object_->UpdatePicking(ray);
}

void TranslationController::OnLostFocus() {
  object_->reset_selected();
}

bool TranslationController::OnMousePressed(const ui::MouseEvent& event) {
  if (!event.IsOnlyLeftMouseButton()) 
    return false;

  if (object_->has_selected()) {
    dragging_ = true;
    Ray ray = context_->GetPickingRay(event.location());
    GetPickingPosOffset(ray, &pos_offset_);
  } else {
    SceneNode* node = context_->GetObjectFromLocation(event.location());
    context_->SetPickingNode(node);
    if (node) {
      UpdateControllerObjectPos();
    }
  }
  return true;
}

void TranslationController::GetPickingPosOffset(const Ray& ray, Vector3* pt) {
  SceneNode* node = context_->GetPickingNode();
  DCHECK(node);
  Vector3 pos = node->position();
  bool parallel;
  switch (picking_object_) {
    case kPickXAxis: {
      Plane pxy(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
      PickingPlane(ray, pxy, pt, &parallel);
      break;
    }
    case kPickYAxis: {
      Plane pyz(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
      PickingPlane(ray, pyz, pt, &parallel);
      break;
    }
    case kPickZAxis: {
      Plane pzx(Vector3(0.0f, 1.0f, 0.0f), -pos.y);
      PickingPlane(ray, pzx, pt, &parallel);
      break;
    }
    case kPickXYPlane: {
      Plane pxy(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
      PickingPlane(ray, pxy, pt, &parallel);
      break;
    }
    case kPickYZPlane: {
      Plane pyz(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
      PickingPlane(ray, pyz, pt, &parallel);
      break;
    }
    case kPickZXPlane: {
      Plane pzx(Vector3(0.0f, 1.0f, 0.0f), -pos.y);
      PickingPlane(ray, pzx, pt, &parallel);
      break;
    }
    default:
      CHECK(false);
      break;
  }
  pt->x = pt->x - pos.x;
  pt->y = pt->y - pos.y;
  pt->z = pt->z - pos.z;
}

bool TranslationController::OnMouseDragged(const ui::MouseEvent& event) {
  if (dragging_) {
    SceneNode* node = context_->GetPickingNode();
    DCHECK(node);
    Ray ray = context_->GetPickingRay(event.location());
    bool parallel;
    Vector3 pt;
    float kMargin = 0.05f;
    DCHECK(picking_object_ != kPickNone);
    Vector3 pos = node->position();
    switch (picking_object_) {
      case kPickXAxis: {
        Plane pxy(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
        PickingPlane(ray, pxy, &pt, &parallel);
        pos.x = pt.x - pos_offset_.x;
        break;
      }
      case kPickYAxis: {
        Plane pyz(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
        PickingPlane(ray, pyz, &pt, &parallel);
        pos.y = pt.y - pos_offset_.y;
        break;
      }
      case kPickZAxis: {
        Plane pzx(Vector3(0.0f, 1.0f, 0.0f), -pos.y);
        PickingPlane(ray, pzx, &pt, &parallel);
        pos.z = pt.z - pos_offset_.z;
        break;
      }
      case kPickXYPlane: {
        Plane pxy(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
        PickingPlane(ray, pxy, &pt, &parallel);
        pos.x = pt.x - pos_offset_.x;
        pos.y = pt.y - pos_offset_.y;
        break;
      }
      case kPickYZPlane: {
        Plane pyz(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
        PickingPlane(ray, pyz, &pt, &parallel);
        pos.y = pt.y - pos_offset_.y;
        pos.z = pt.z - pos_offset_.z;
        break;
      }
      case kPickZXPlane: {
        Plane pzx(Vector3(0.0f, 1.0f, 0.0f), -pos.y);
        PickingPlane(ray, pzx, &pt, &parallel);
        pos.x = pt.x - pos_offset_.x;
        pos.z = pt.z - pos_offset_.z;
        break;
      }
      default:
        CHECK(false);
        break;
    }

    object_->SetPosition(pos);
    node->SetPosition(pos);
  }
  
  return true;
}

bool TranslationController::OnMouseReleased(const ui::MouseEvent& event) {
  dragging_ = false;
  return false;
}

void TranslationController::OnMouseMoved(const ui::MouseEvent& event) {
  if (context_->GetPickingNode()) {
    UpdateControllerObjectState(event.location());
  } else {
    object_->reset_selected();
  }
}

void TranslationController::UpdateControllerObjectPos() {
  SceneNode* node = context_->GetPickingNode();
  DCHECK(node);
  float radius = node->vmin().distance(node->vmax()) * 0.5f;
  object_->set_length(radius * kAxisLengthMultiply);
  object_->reset_selected();
  object_->SetPosition(node->position());
}

TransformAxisObject::TransformAxisObject(DiffuseEffect* effect) 
    : length_(1.0f),
      effect_(effect) {
  Plane xzplane = Plane(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
  rotation_[0] = std::move(MirrorTrans(xzplane)) * 
      std::move(RotateZ(Degree(90.0f)));
  rotation_[1] = Matrix4::kIdentity;
  rotation_[2] = std::move(MirrorTrans(xzplane)) * 
      std::move(RotateX(Degree(-90.0f)));
  reset_color();
  CreatePlane(effect_->GetVertexDesc());
  CreatePlaneFrame(effect_->GetVertexDesc());
}

TransformAxisObject::~TransformAxisObject() {
}

void TransformAxisObject::reset_color() {
  color_[0] = Vector4(1.0f, 0.0f, 0.0f, 0.3f);
  color_[1] = Vector4(0.0f, 1.0f, 0.0f, 0.3f);
  color_[2] = Vector4(0.0f, 0.0f, 1.0f, 0.3f);
}

void TransformAxisObject::set_color(const Vector4& color, int32 axis) {
  color_[axis] = color;
}

void TransformAxisObject::CreatePlane(azer::VertexDesc* desc) {
  const float v = kPlaneLength * length();
  const Vector4 pos[] = {
    Vector4(0.0f, 0.0f, 0.0f, 1.0f),
    Vector4(   v, 0.0f, 0.0f, 1.0f),
    Vector4(   v, 0.0f,    v, 1.0f),
    Vector4(0.0f, 0.0f,    v, 1.0f),
  };
  plane_ = lord::CreatePlane(pos, desc);
}

void TransformAxisObject::CreatePlaneFrame(azer::VertexDesc* desc) {
  float v = kPlaneLength * length();
  Vector4 pos[] = {
    Vector4(0.0f, 0.0f, v,    1.0f),
    Vector4(   v, 0.0f, v,    1.0f),
    Vector4(   v, 0.0f, v,    1.0f),
    Vector4(   v, 0.0f, 0.0f, 1.0f),
  };
  plane_frame_ = CreateLineList(pos, (int32)arraysize(pos), desc);
}

void TransformAxisObject::set_length(float length) {
  length_ = std::max(length, kControllerMinScale);
  CreatePlane(effect_->GetVertexDesc());
  CreatePlaneFrame(effect_->GetVertexDesc());
}

void TransformAxisObject::Render(const Matrix4& pv, azer::Renderer* renderer) {
  Context* context = Context::instance();
  bool depth_enable = renderer->IsDepthTestEnable();
  CullingMode culling = renderer->GetCullingMode();
  Matrix4 world = Translate(position_);

  int32 count = static_cast<int32>(arraysize(rotation_));
  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  renderer->EnableDepthTest(false);
  renderer->SetCullingMode(kCullNone);
  for (int32 i = 0; i < count; ++i) {
    Matrix4 lworld = std::move(world * rotation_[i]);
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetPV(pv);
    effect_->SetWorld(lworld);
    effect_->SetColor(color_[i]);
    renderer->UseEffect(effect_.get());
    plane_->Draw(renderer);
  }

  renderer->ResetBlending();

  for (int32 i = 0; i < count; ++i) {
    Matrix4 lworld = std::move(world * rotation_[i]);
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetPV(pv);
    effect_->SetWorld(lworld);
    effect_->SetColor(color_[i]);
    renderer->UseEffect(effect_.get());
    plane_frame_->Draw(renderer);
  }

  renderer->EnableDepthTest(depth_enable);
  renderer->SetCullingMode(culling);
}

int32 TransformAxisObject::Picking(const azer::Ray& ray) const {
  const Vector3& pos = position_;
  Plane pxy(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
  Plane pyz(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
  Plane pzx(Vector3(0.0f, 1.0f, 0.0f), -pos.y);
  
  bool parallel;
  Vector3 pt;
  float kMargin = 0.05f;
  float length = kPlaneLength * length_;
  {
    // xyplane
    PickingPlane(ray, pxy, &pt, &parallel);
    if (!parallel) {
      if (pt.x - pos.x <= length && pt.x - pos.x > 0.0f
          && pt.y - pos.y < length && pt.y - pos.y > 0.0f) {
        return kPickXYPlane;
      }
    }
  }
  
  {
    // yzplane
    PickingPlane(ray, pyz, &pt, &parallel);
    if (!parallel) {
      if (pt.y - pos.y <= length && pt.y - pos.y > 0.0f
          && pt.z - pos.z < length && pt.z - pos.z > kMargin) {
        return kPickYZPlane;
      }
    }
  }

  {
    // zxplane
    PickingPlane(ray, pzx, &pt, &parallel);
    if (!parallel) {
      if (pt.z - pos.z <= length && pt.z - pos.z > 0.0f
          && pt.x - pos.x < length && pt.x - pos.x > kMargin) {
        return kPickZXPlane;
      }
    }
  }

  return kPickNone;
}

// class TranslationControllerObject
TranslationControllerObject::TranslationControllerObject() {
  selected_color_ = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
  DiffuseEffectPtr effect = CreateDiffuseEffect();
  plane_.reset(new TransformAxisObject(effect.get()));
  axis_.reset(new XYZAxisObject(effect.get()));
  SetPosition(Vector3(0.0f, 0.0f, 0.0f));
}

TranslationControllerObject::~TranslationControllerObject() {
}

void TranslationControllerObject::reset_selected() {
  memset(selected_axis_, 0, sizeof(selected_axis_));
  memset(selected_plane_, 0, sizeof(selected_plane_));
  plane_->reset_color();
  axis_->reset_color();
}

void TranslationControllerObject::set_selected_axis(int32 axis) {
  DCHECK(axis < static_cast<int32>(arraysize(selected_axis_)));
  selected_axis_[axis] = 1;
  axis_->set_color(selected_color_, axis);
}

void TranslationControllerObject::set_selected_plane(int32 plane) {
  DCHECK(plane < static_cast<int32>(arraysize(selected_plane_)));
  selected_plane_[plane] = 1;
  plane_->set_color(selected_color_, plane);
}

const azer::Vector3& TranslationControllerObject::position() const {
  return axis_->position();
}

void TranslationControllerObject::SetPosition(const Vector3& position) {
  axis_->SetPosition(position);
  plane_->SetPosition(position);
}

void TranslationControllerObject::set_length(float scale) {
  plane_->set_length(scale);
  axis_->set_length(scale);
}

void TranslationControllerObject::Render(const Matrix4& pv, Renderer* renderer) {
  Context* context = Context::instance();
  axis_->Render(pv, renderer);
  plane_->Render(pv, renderer);
}

int32 TranslationControllerObject::Picking(const azer::Ray& ray) const {
  int ret = plane_->Picking(ray);
  if (ret != kPickNone)
    return ret;
  
  return axis_->Picking(ray);
}

int32 TranslationControllerObject::UpdatePicking(const azer::Ray& ray) {
  reset_selected();
  int32 target = Picking(ray);
  switch (target) {
    case kPickXYPlane:
      set_selected_axis(0);
      set_selected_axis(1);
      set_selected_plane(2);
      break;
    case kPickYZPlane:
      set_selected_axis(1);
      set_selected_axis(2);
      set_selected_plane(0);
      break;
    case kPickZXPlane:
      set_selected_axis(2);
      set_selected_axis(0);
      set_selected_plane(1);
    case kPickXAxis:
      set_selected_axis(0);
      break;
    case kPickYAxis:
      set_selected_axis(1);
      break;
    case kPickZAxis:
      set_selected_axis(2);
      break;
  }
  return target;
}
}  // namespace lord
