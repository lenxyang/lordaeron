#include "lordaeron/interactive/translation_controller.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/interactive/axis_object.h"
#include "lordaeron/interactive/controller_object.h"
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
    : dragging_(false) {
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
  object_->reset_selected();
  Vector3 pos = (node->vmin() + node->vmax()) * 0.5f;
  float radius = node->vmin().distance(node->vmax()) * 0.5f;
  Ray ray = context_->GetPickingRay(location);
  Plane pxy(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
  Plane pyz(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
  Plane pzx(Vector3(0.0f, 1.0f, 0.0f), -pos.y);
  
  bool parallel;
  Vector3 pt;
  float kMargin = 0.05f;
  float axis_length = object_->length();
  float square_length = axis_length * kPlaneLength;
  {
    // xyplane
    PickingPlane(ray, pxy, &pt, &parallel);
    if (!parallel) {
      if (pt.x - pos.x <= square_length && pt.x - pos.x > 0.0f
          && pt.y - pos.y < square_length && pt.y - pos.y > 0.0f) {
        object_->set_selected_axis(0);
        object_->set_selected_axis(1);
        object_->set_selected_plane(2);
        return;
      } else if (pt.x - pos.x <= axis_length && pt.x - pos.x > 0.0 &&
                 std::abs(pt.y - pos.y) < 0.01 && std::abs(pt.y - pos.y) < kMargin) {
        object_->set_selected_axis(0);
        return;
      } else if (pt.y - pos.y <= axis_length && pt.y - pos.y > 0.0f &&
                 std::abs(pt.x - pos.x) < 0.01 && std::abs(pt.x - pos.x) < kMargin) {
        object_->set_selected_axis(1);
        return;
      } else {
        // find next plane
      }
    }
  }
  
  {
    // yzplane
    PickingPlane(ray, pyz, &pt, &parallel);
    if (!parallel) {
      if (pt.y - pos.y <= square_length && pt.y - pos.y > 0.0f
          && pt.z - pos.z < square_length && pt.z - pos.z > kMargin) {
        object_->set_selected_axis(1);
        object_->set_selected_axis(2);
        object_->set_selected_plane(0);
        return;
      } else if (pt.y - pos.y <= axis_length && pt.y - pos.y > 0.0 &&
                 std::abs(pt.z - pos.z) < 0.01 && std::abs(pt.z - pos.z) < kMargin) {
        object_->set_selected_axis(1);
        return;
      } else if (pt.z - pos.z <= axis_length && pt.z - pos.z > 0.0f &&
                 std::abs(pt.y - pos.y) < 0.01 && std::abs(pt.y - pos.y) < kMargin) {
        object_->set_selected_axis(2);
        return;
      } else {
      }
    }
  }

  {
    // zxplane
    PickingPlane(ray, pzx, &pt, &parallel);
    if (!parallel) {
      if (pt.z - pos.z <= square_length && pt.z - pos.z > 0.0f
          && pt.x - pos.x < square_length && pt.x - pos.x > kMargin) {
        object_->set_selected_axis(2);
        object_->set_selected_axis(0);
        object_->set_selected_plane(1);
        return;
      } else if (pt.z - pos.z <= axis_length && pt.z - pos.z > 0.0 &&
                 std::abs(pt.x - pos.x) < 0.01 && std::abs(pt.x - pos.x) < kMargin) {
        object_->set_selected_axis(2);
        return;
      } else if (pt.x - pos.x <= axis_length && pt.x - pos.x > 0.0f &&
                 std::abs(pt.z - pos.z) < 0.01 && std::abs(pt.z - pos.z) < kMargin) {
        object_->set_selected_axis(0);
        return;
      } else {
      }
    }
  }
}

void TranslationController::OnLostFocus() {
  object_->reset_selected();
}

bool TranslationController::OnMousePressed(const ui::MouseEvent& event) {
  SceneNode* node = NULL;
  if (object_->has_selected()) {
    return true;
  } else {
    node = context_->GetObjectFromLocation(event.location());
    context_->SetPickingNode(node);
    UpdateControllerObjectPos();
  }
  return true;
}

bool TranslationController::OnMouseDragged(const ui::MouseEvent& event) {
  return false;
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
  Vector3 pos = (node->vmin() + node->vmax()) * 0.5f;
  float radius = node->vmin().distance(node->vmax()) * 0.5f;
  object_->set_length(radius * kAxisLengthMultiply);
  object_->reset_selected();
  object_->SetPosition(pos);
}

TransformAxisObject::TransformAxisObject(azer::VertexDesc* desc) 
    : length_(1.0f),
      desc_(desc) {
  CreatePlaneFrame(desc);
}

TransformAxisObject::~TransformAxisObject() {
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
  length_ = length;
  CreatePlane(desc_.get());
}

void TransformAxisObject::RenderPlane(azer::Renderer* renderer) {
  Context* context = Context::instance();
  bool depth_enable = renderer->IsDepthTestEnable();
  CullingMode culling = renderer->GetCullingMode();

  plane_frame_->Draw(renderer);

  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  renderer->EnableDepthTest(false);
  renderer->SetCullingMode(kCullNone);
  plane_->Draw(renderer);

  renderer->ResetBlending();
  renderer->EnableDepthTest(depth_enable);
  renderer->SetCullingMode(culling);
}

// class TranslationControllerObject
TranslationControllerObject::TranslationControllerObject() {
  selected_color_ = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
  effect_ = CreateDiffuseEffect();
  plane_.reset(new TransformAxisObject(effect_->GetVertexDesc()));
  color_[0] = Vector4(1.0f, 0.0f, 0.0f, 0.3f);
  color_[1] = Vector4(0.0f, 1.0f, 0.0f, 0.3f);
  color_[2] = Vector4(0.0f, 0.0f, 1.0f, 0.3f);
  Plane xzplane = Plane(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
  rotation_[0] = std::move(MirrorTrans(xzplane)) * 
      std::move(RotateZ(Degree(90.0f)));
  rotation_[1] = Matrix4::kIdentity;
  rotation_[2] = std::move(MirrorTrans(xzplane)) * 
      std::move(RotateX(Degree(-90.0f)));
  axis_.reset(new XYZAxisObject(effect_.get()));
  SetPosition(Vector3(0.0f, 0.0f, 0.0f));
}

TranslationControllerObject::~TranslationControllerObject() {
}

void TranslationControllerObject::reset_selected() {
  memset(selected_axis_, 0, sizeof(selected_axis_));
  memset(selected_plane_, 0, sizeof(selected_plane_));
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
}

void TranslationControllerObject::SetPosition(const Vector3& position) {
  world_ = Translate(position);
  axis_->SetPosition(position);
}

void TranslationControllerObject::set_length(float scale) {
  plane_->set_length(scale);
  axis_->set_length(scale);
}

void TranslationControllerObject::Render(const Matrix4& pv, Renderer* renderer) {
  Context* context = Context::instance();
  axis_->SetPV(pv);
  axis_->Render(renderer);

  int32 count = static_cast<int32>(arraysize(rotation_));
  for (int32 i = 0; i < count; ++i) {
    Matrix4 world = std::move(world_ * rotation_[i]);
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetPV(pv);
    effect_->SetWorld(world);
    effect_->SetColor(selected_plane_[i] ? selected_color_ : color_[i]);
    renderer->UseEffect(effect_.get());
    plane_->RenderPlane(renderer);
  }
}
}  // namespace lord
