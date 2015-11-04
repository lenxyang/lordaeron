#include "lordaeron/interactive/translation_controller.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/interactive/axis_object.h"
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
  if (object_->has_axis_selected()) {
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
  axis_.reset(new ::lord::AxisObject(desc));
  CreatePlane(desc);
  CreatePlaneFrame(desc);
}

TransformAxisObject::~TransformAxisObject() {
}

void TransformAxisObject::CreatePlane(azer::VertexDesc* desc) {
  VertexPos normal_pos;
  bool kHasNormal0Idx = GetSemanticIndex("normal", 0, desc, &normal_pos);
  SlotVertexDataPtr vdata = new SlotVertexData(desc, 6);
  VertexPack vpack(vdata.get());
  
  vpack.first();
  const float v = kPlaneLength * length();
  const Vector4 pos[] = {
    Vector4(0.0f, 0.0f, 0.0f, 1.0f),
    Vector4(0.0f, 0.0f,    v, 1.0f),
    Vector4(   v, 0.0f,    v, 1.0f),

    Vector4(0.0f, 0.0f, 0.0f, 1.0f),
    Vector4(   v, 0.0f,    v, 1.0f),
    Vector4(   v, 0.0f, 0.0f, 1.0f),
  };
  for (uint32 i = 0; i < arraysize(pos); ++i) {
    vpack.WriteVector4(pos[i], VertexPos(0, 0));
    if (kHasNormal0Idx)
      vpack.WriteVector4(Vector4(0.0f, 1.0f, 0.0f, 0.0f), normal_pos);
    vpack.next(1);
  }

  RenderSystem* rs = RenderSystem::Current();
  VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
  plane_ = new Entity;
  plane_->SetVertexBuffer(vb);
}

void TransformAxisObject::CreatePlaneFrame(azer::VertexDesc* desc) {
  float v = kPlaneLength * length();
  Vector4 pos[] = {
    Vector4(0.0f, 0.0f, v,    1.0f),
    Vector4(   v, 0.0f, v,    1.0f),
    Vector4(   v, 0.0f, v,    1.0f),
    Vector4(   v, 0.0f, 0.0f, 1.0f),
  };
  Vector4 normal(1.0f, 1.0f, 1.0f, 0.0f);

  VertexPos normal_pos;
  bool kHasNormal0Idx = GetSemanticIndex("normal", 0, desc, &normal_pos);
  SlotVertexDataPtr vdata = new SlotVertexData(desc, arraysize(pos));
  VertexPack vpack(vdata.get());
  vpack.first();
  for (int32 i = 0; i < arraysize(pos); ++i) {
    vpack.WriteVector4(pos[i], VertexPos(0, 0));
    if (kHasNormal0Idx)
      vpack.WriteVector4(normal, normal_pos);
    vpack.next(1);
  }

  RenderSystem* rs = RenderSystem::Current();
  VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
  plane_frame_ = new Entity;
  plane_frame_->set_topology(kLineList);
  plane_frame_->SetVertexBuffer(vb);
}

void TransformAxisObject::set_length(float length) {
  length_ = length;
  axis_->set_length(length);
  CreatePlane(desc_.get());
}

void TransformAxisObject::RenderAxis(azer::Renderer* renderer) {
  axis_->Render(renderer);
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
  axis_.reset(new TransformAxisObject(effect_->GetVertexDesc()));
  color_[0] = Vector4(1.0f, 0.0f, 0.0f, 0.3f);
  color_[1] = Vector4(0.0f, 1.0f, 0.0f, 0.3f);
  color_[2] = Vector4(0.0f, 0.0f, 1.0f, 0.3f);
  Plane xzplane = Plane(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
  rotation_[0] = std::move(MirrorTrans(xzplane)) * 
      std::move(RotateZ(Degree(90.0f)));
  rotation_[1] = Matrix4::kIdentity;
  rotation_[2] = std::move(MirrorTrans(xzplane)) * 
      std::move(RotateX(Degree(-90.0f)));

  SetPosition(Vector3(0.0f, 0.0f, 0.0f));
}

TranslationControllerObject::~TranslationControllerObject() {
}

void TranslationControllerObject::reset_selected() {
  memset(selected_axis_, 0, sizeof(selected_axis_));
  memset(selected_plane_, 0, sizeof(selected_plane_));
}

void TranslationControllerObject::set_selected_axis(int32 axis) {
  DCHECK(axis < static_cast<int32>(arraysize(selected_axis_)));
  selected_axis_[axis] = 1;
}

void TranslationControllerObject::set_selected_plane(int32 plane) {
  DCHECK(plane < static_cast<int32>(arraysize(selected_plane_)));
  selected_plane_[plane] = 1;
}

void TranslationControllerObject::SetPosition(const Vector3& position) {
  world_ = Translate(position);
}

void TranslationControllerObject::set_length(float scale) {
  axis_->set_length(scale);
}

void TranslationControllerObject::Render(const Matrix4& pv, Renderer* renderer) {
  Context* context = Context::instance();
  int32 count = static_cast<int32>(arraysize(rotation_));
  for (int32 i = 0; i < count; ++i) {
    Matrix4 world = std::move(world_ * rotation_[i]);
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetColor(selected_axis_[i] ? selected_color_ : color_[i]);
    effect_->SetPV(pv);
    effect_->SetWorld(world);
    renderer->UseEffect(effect_.get());
    axis_->RenderAxis(renderer);

    effect_->SetColor(selected_plane_[i] ? selected_color_ : color_[i]);
    renderer->UseEffect(effect_.get());
    axis_->RenderPlane(renderer);
  }
}
}  // namespace lord
