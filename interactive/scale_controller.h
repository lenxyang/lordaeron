#pragma once

#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/interactive_controller.h"

namespace lord {
class XYZAxisObject;
class InteractiveContext;
class SceneNode;
class ScaleControllerObject;

class ScaleController : public InteractiveController {
 public:
  ScaleController();
  ~ScaleController() override;

  // override
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
  void OnOperationStart(InteractiveContext* ctx) override;
  void OnOperationStop() override;
  void OnLostFocus() override;
  bool OnMousePressed(const ui::MouseEvent& event) override;
  bool OnMouseDragged(const ui::MouseEvent& event) override;
  bool OnMouseReleased(const ui::MouseEvent& event) override;
  void OnMouseMoved(const ui::MouseEvent& event) override;

  void UpdateControllerObjectPos();
  void UpdateControllerObjectState(const gfx::Point& pt);
 private:
  scoped_ptr<ScaleControllerObject> object_;
  azer::Vector3 origin_scale_;
  bool dragging_;
  DISALLOW_COPY_AND_ASSIGN(ScaleController);
};

class ScaleAxisPlaneObject {
 public:
  ScaleAxisPlaneObject(DiffuseEffect *effect);
  ~ScaleAxisPlaneObject();

  void set_length(float v);
  void set_outer(float v) { outer_ = v;}
  void set_inner(float v) { inner_ = v;}

  void SetPosition(const azer::Vector3& pos);
  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);
 private:
  void InitPlane();
  void InitPlaneFrame();
  float axis_length_;
  float inner_;
  float outer_;
  azer::Vector4 color_[3];
  azer::Matrix4 rotation_[3];
  azer::EntityPtr plane_;
  azer::EntityPtr plane_frame_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(ScaleAxisPlaneObject);
};

class ScaleControllerObject {
 public:
  ScaleControllerObject();
  ~ScaleControllerObject();

  void SetPV(const azer::Matrix4& pv);
  void SetPosition(const azer::Vector3& pos);
  void Render(azer::Renderer* renderer);

  void reset_selected();
  void set_selected_axis(int32 axis);
  void set_selected_plane(int32 axis);
 private:
  bool selected_axis_[3];
  bool selected_plane_[3];
  DiffuseEffectPtr effect_;
  scoped_ptr<XYZAxisObject> axis_;
  scoped_ptr<ScaleAxisPlaneObject> plane_;
  DISALLOW_COPY_AND_ASSIGN(ScaleControllerObject);
};
}  // namespace lord
