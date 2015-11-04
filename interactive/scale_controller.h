#pragma once

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
  ScaleAxisPlaneObject(DiffuseEffectPtr *effect);
  ~ScaleAxisPlaneObject();

  void set_outer(float v) { outer_ = v;}
  void set_inner(float v) { inner_ = v;}

  void SetPosition(const Vector3& pos);
  void SetPV(const azer::Matrix4& pv);
  void Render(azer::Renderer* renderer);
 private:
  void InitPlane();
  void InitPlaneFrame();
  float inner_;
  float outer_;
  azer::Vector4 color_[3];
  azer::Matrix4 rotation_[3];
  zer::EntityPtr plane_;
  zer::EntityPtr plane_frame;
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
 private:
  DiffuseEffectPtr effect_;
  scoped_ptr<XYZAxisObject> axis_;
  DISALLOW_COPY_AND_ASSIGN(ScaleControllerObject);
};
}  // namespace lord
