#include "lordaeron/interactive/scale_controller.h"

#include "base/logging.h"
#include "lordaeron/env.h"
#include "lordaeron/interactive/axis_object.h"
#include "lordaeron/interactive/constants.h"
#include "lordaeron/interactive/controller_object.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/interactive/scale_controller.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/render_window.h"
#include "lordaeron/util/picking.h"

namespace lord {

// class TransformAxisObject
namespace {
const float kPlaneLength = 0.20f;
const float kAxisLengthMultiply = 1.15f;
}

using namespace azer;


ScaleController::ScaleController() {
  object_.reset(new ScaleControllerObject);
}

ScaleController::~ScaleController() {
}

  // override
void ScaleController::Update(const FrameArgs& args) {
}

void ScaleController::Render(Renderer* renderer) {
  SceneNode* node = context_->GetPickingNode();
  if (node) {
    const Camera& camera = context_->window()->camera();
    const Matrix4& pv = camera.GetProjViewMatrix();
    object_->SetPV(pv);
    object_->Render(renderer);
  }
}

void ScaleController::OnOperationStart(InteractiveContext* ctx) {
  SceneNode* node = context_->GetPickingNode();
  if (node) {
    UpdateControllerObjectPos();
  }
}

void ScaleController::OnOperationStop() {
}

void ScaleController::OnLostFocus() {
  object_->reset_selected();
}

bool ScaleController::OnMousePressed(const ui::MouseEvent& event) {
  if (!event.IsOnlyLeftMouseButton()) 
    return false;

  SceneNode* node = context_->GetObjectFromLocation(event.location());
  context_->SetPickingNode(node);
  if (node) {
    float radius = node->vmin().distance(node->vmax()) * 0.5f;
    object_->set_length(radius * kAxisLengthMultiply);
    object_->SetPosition(node->position());
  }
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
  const float kAxisLengthMultiply = 1.1f;
  SceneNode* node = context_->GetPickingNode();
  DCHECK(node);
  float radius = node->vmin().distance(node->vmax()) * 0.5f;
  object_->set_length(radius * kAxisLengthMultiply);
  object_->reset_selected();
  object_->SetPosition(node->position());
}

// class ScaleAxisPlaneObject
ScaleAxisPlaneObject::ScaleAxisPlaneObject(DiffuseEffect* effect)
    : effect_(effect),
      axis_length_(1.0f),
      inner_(0.35f),
      outer_(0.55f) {
  Plane xzplane = Plane(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
  rotation_[0] = std::move(MirrorTrans(xzplane)) * 
      std::move(RotateZ(Degree(90.0f)));
  rotation_[1] = Matrix4::kIdentity;
  rotation_[2] = std::move(MirrorTrans(xzplane)) * 
      std::move(RotateX(Degree(-90.0f)));
  reset_color();
  set_length(1.0f);

  render_state_ = RenderSystem::Current()->CreateRenderState();
  render_state_->SetCullingMode(kCullNone);
}

ScaleAxisPlaneObject::~ScaleAxisPlaneObject() {
}

void ScaleAxisPlaneObject::reset_color() {
  color_[0] = Vector4(0.0f, 0.0f, 1.0f, 0.3f);  // xy
  color_[1] = Vector4(0.0f, 1.0f, 0.0f, 0.3f);  // yz
  color_[2] = Vector4(1.0f, 0.0f, 0.0f, 0.3f);  // zy
  color_[3] = Vector4(1.0f, 1.0f, 1.0f, 0.3f);  // zy
}

void ScaleAxisPlaneObject::set_color(const Vector4& color, int32 index) {
  DCHECK(index < arraysize(color_));
  color_[index] = color;
}

void ScaleAxisPlaneObject::set_length(float l) {
  float length = std::max(l, kControllerMinScale);
  axis_length_ = length;
  InitPlane();
  InitPlaneFrame();
  InitCenterPlane();
}

void ScaleAxisPlaneObject::SetPosition(const Vector3& pos) {
  world_ = Translate(pos);
}

void ScaleAxisPlaneObject::InitPlane() {
  Vector4 pos[4] = {
    Vector4(outer_ * axis_length_, 0.0f, 0.0f, 1.0f),
    Vector4(0.0f, 0.0f, outer_ * axis_length_, 1.0f),
    Vector4(0.0f, 0.0f, inner_ * axis_length_, 1.0f),
    Vector4(inner_ * axis_length_, 0.0f, 0.0f, 1.0f),
  };
  plane_ = lord::CreatePlane(pos, effect_->vertex_desc());
}

void ScaleAxisPlaneObject::InitCenterPlane() {
  Vector4 pos[4] = {
    Vector4(0.0f, 0.0f, inner_ * axis_length_, 1.0f),
    Vector4(0.0f, inner_ * axis_length_, 0.0f, 1.0f),
    Vector4(inner_ * axis_length_, 0.0f, 0.0f, 1.0f),
    Vector4(0.0f, 0.0f, inner_ * axis_length_, 1.0f),
  };
  center_plane_ = lord::CreatePlane(pos, effect_->vertex_desc());
}

void ScaleAxisPlaneObject::InitPlaneFrame() {
  Vector4 pos[4] = {
    Vector4(outer_ * axis_length_, 0.0f, 0.0f, 1.0f),
    Vector4(0.0f, 0.0f, outer_ * axis_length_, 1.0f),
    Vector4(0.0f, 0.0f, inner_ * axis_length_, 1.0f),
    Vector4(inner_ * axis_length_, 0.0f, 0.0f, 1.0f),
  };
  plane_frame_ = CreateLineList(pos, (int32)arraysize(pos), effect_->vertex_desc());
}

void ScaleAxisPlaneObject::Render(const azer::Matrix4& pv, Renderer* renderer) {
  LordEnv* context = LordEnv::instance();
  bool depth_enable = renderer->IsDepthTestEnable();

  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  renderer->EnableDepthTest(false);
  ScopedRenderState scoped_render_state(renderer, render_state_);
  for (int32 i = 0; i < 3; ++i) {
    Matrix4 lworld = std::move(world_ * rotation_[i]);
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetColor(color_[i]);
    effect_->SetPV(pv);
    effect_->SetWorld(lworld);
    renderer->UseEffect(effect_.get());
    plane_->Render(renderer);
  }

  effect_->SetDirLight(context->GetInternalLight());
  effect_->SetColor(color_[3]);
  effect_->SetPV(pv);
  effect_->SetWorld(world_);
  renderer->UseEffect(effect_.get());
  center_plane_->Render(renderer);
  renderer->ResetBlending();

  for (int32 i = 0; i < 3; ++i) {
    Matrix4 lworld = std::move(world_ * rotation_[i]);
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetColor(color_[i]);
    effect_->SetPV(pv);
    effect_->SetWorld(lworld);
    renderer->UseEffect(effect_.get());
    plane_frame_->Render(renderer);
  }

  renderer->EnableDepthTest(depth_enable);
}


// class ScaleControllerObject
ScaleControllerObject::ScaleControllerObject() {
  effect_ = CreateDiffuseEffect();
  axis_.reset(new XYZAxisObject(effect_.get()));
  plane_.reset(new ScaleAxisPlaneObject(effect_.get()));
}

ScaleControllerObject::~ScaleControllerObject() {
}

void ScaleControllerObject::set_length(float length) {
  axis_->set_length(length);
  plane_->set_length(length);
}

void ScaleControllerObject::reset_selected() {
  memset(selected_axis_, 0, sizeof(selected_axis_));
  memset(selected_plane_, 0, sizeof(selected_plane_));
}

void ScaleControllerObject::set_selected_axis(int32 axis) {
  DCHECK(axis < arraysize(selected_axis_));
  selected_axis_[axis] = 1;
}

void ScaleControllerObject::set_selected_plane(int32 plane) {
DCHECK(plane < arraysize(selected_plane_));
  selected_plane_[plane] = 1;
}

void ScaleControllerObject::SetPV(const Matrix4& pv) {
  pv_ = pv;
}

void ScaleControllerObject::SetPosition(const Vector3& pos) {
  axis_->SetPosition(pos);
  plane_->SetPosition(pos);
}

void ScaleControllerObject::Render(Renderer* renderer) {
  axis_->Render(pv_, renderer);
  plane_->Render(pv_, renderer);
}
}  // namespace lord
