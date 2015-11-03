#include "lordaeron/interactive/translation_controller.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/util/picking.h"

namespace lord {

// class AxisTransformObject
namespace {
const float kConeHeight = 0.1f;
const float kConeRadius = 0.03f;
const float kPlaneLength = 0.20f;
}

using namespace azer;
TranslationController::TranslationController() 
    : dragging_(false) {
  object_.reset(new TranslationControllerObject);
  object_->set_scale(3.0f);
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

void TranslationController::UpdateControllerObjectState(const gfx::Point& location) {
  SceneNode* node = context_->GetPickingNode();
  DCHECK(node);
  object_->reset_selected();
  Vector3 pos = (node->vmin() + node->vmax()) * 0.5f;
  float radius = node->vmin().distance(node->vmax()) * 0.5f;
  Ray ray = context_->GetPickingRay(location);
  Plane px(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
  Plane py(Vector3(0.0f, 1.0f, 0.0f), -pos.y);
  Plane pz(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
  
  bool parallel;
  Vector3 pt;
  float length = kPlaneLength * radius + 0.01f;
  {
    // xplane
    PickingPlane(ray, px, &pt, &parallel);
    if (parallel) {
      const Camera& camera = context_->window()->camera();
      pt = camera.holder().position();
      if (pt.z - pos.z <= length && pt.y - pos.y < length) {
        object_->set_selected(1);
        object_->set_selected(2);
      } else if (pt.y - pos.y <= radius) {
        object_->set_selected(1);
      } else if (pt.z - pos.z <= radius) {
        object_->set_selected(2);
      }
    } else {
      if (pt.z - pos.z <= length && pt.y - pos.y < length) {
        object_->set_selected(1);
        object_->set_selected(2);
      } else if (pt.y - pos.y <= radius) {
        object_->set_selected(1);
      } else if (pt.z - pos.z <= radius) {
        object_->set_selected(2);
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
  } else {
    node = context_->GetObjectFromLocation(event.location());
    context_->SetPickingNode(node);
    Vector3 pos = (node->vmin() + node->vmax()) * 0.5f;
    float radius = node->vmin().distance(node->vmax()) * 0.5f;
    object_->set_scale(radius * 1.3f);
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

AxisTransformObject::AxisTransformObject(azer::VertexDesc* desc) 
    : length_(1.0f),
      desc_(desc) {
  CreateCone(desc);
  CreateLine(desc);
  CreatePlane(desc);
}

AxisTransformObject::~AxisTransformObject() {
}

void AxisTransformObject::CreateCone(azer::VertexDesc* desc) {
  SlotVertexDataPtr vdata = InitConeVertexData(32, VertexDescPtr(desc));
  VertexPack vpack(vdata.get());
  vpack.first();
  Vector4 pos;
  VertexPos posidx(0, 0);
  while (!vpack.end()) {
    vpack.ReadVector4(&pos, posidx);
    pos = Vector4(pos.x * kConeRadius, pos.y * kConeHeight, pos.z * kConeRadius,
                  1.0f);
    pos.y = pos.y + 0.9f * length();
    vpack.WriteVector4(pos, posidx);
    vpack.next(1);
  }

  IndicesDataPtr idata = InitConeIndicesData(32);
  RenderSystem* rs = RenderSystem::Current();
  VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
  IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
  cone_ = new Entity(vb, ib);
}

void AxisTransformObject::CreatePlane(azer::VertexDesc* desc) {
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

void AxisTransformObject::CreateLine(azer::VertexDesc* desc) {
  float v = kPlaneLength * length();
  Vector4 pos[] = {
    Vector4(0.0f, 0.0f, 0.0f, 1.0f),
    Vector4(0.0f, 0.9f * length(), 0.0f, 1.0f),
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
  line_ = new Entity;
  line_->set_topology(kLineList);
  line_->SetVertexBuffer(vb);
}

void AxisTransformObject::set_length(float length) {
  length_ = length;
  CreateCone(desc_.get());
  CreateLine(desc_.get());
  CreatePlane(desc_.get());
}

void AxisTransformObject::Render(azer::Renderer* renderer) {
  cone_->DrawIndex(renderer);

  bool depth_enable = renderer->IsDepthTestEnable();
  CullingMode culling = renderer->GetCullingMode();
  renderer->EnableDepthTest(false);
  renderer->SetCullingMode(kCullNone);
  line_->Draw(renderer);

  Context* context = Context::instance();
  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  plane_->Draw(renderer);
  renderer->ResetBlending();
  renderer->EnableDepthTest(depth_enable);
  renderer->SetCullingMode(culling);
}

// class TranslationControllerObject
TranslationControllerObject::TranslationControllerObject() {
  selected_color_ = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
  effect_ = CreateDiffuseEffect();
  object_.reset(new AxisTransformObject(effect_->GetVertexDesc()));
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
}

void TranslationControllerObject::set_selected(int32 axis) {
  DCHECK(axis < static_cast<int32>(arraysize(selected_axis_)));
  selected_axis_[axis] = 1;
}

void TranslationControllerObject::SetPosition(const Vector3& position) {
  world_ = Translate(position);
}

void TranslationControllerObject::set_scale(float scale) {
  object_->set_length(scale);
}

void TranslationControllerObject::Render(const Matrix4& pv, Renderer* renderer) {
  Context* context = Context::instance();
  for (int32 i = 0; i < arraysize(rotation_); ++i) {
    Matrix4 world = std::move(world_ * rotation_[i]);
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetColor(selected_axis_[i] ? selected_color_ : color_[i]);
    effect_->SetPV(pv);
    effect_->SetWorld(world);
    renderer->UseEffect(effect_.get());
    object_->Render(renderer);
  }
}
}  // namespace lord
