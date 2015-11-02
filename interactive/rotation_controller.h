#pragma once

#include "base/memory/scoped_ptr.h"
#include "lordaeron/interactive/interactive_controller.h"

namespace lord {
class DiffuseEffect;
class InteractiveContext;
class SceneNode;
class RotationControllerObject;
typedef scoped_refptr<DiffuseEffect> DiffuseEffectPtr;

class RotationController : public InteractiveController {
 public:
  RotationController();
  ~RotationController() override;

  enum {
    kAxisNone,
    kAxisX,
    kAxisY,
    kAxisZ,
  };

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
 private:
  int32 GetSelectedAxis(gfx::Point location);
  scoped_ptr<RotationControllerObject> object_;
  int32 rotating_axis_;
  DISALLOW_COPY_AND_ASSIGN(RotationController);
};

class CircleCoordinateObject {
 public:
  CircleCoordinateObject(DiffuseEffect* effect);
  ~CircleCoordinateObject();

  float radius() const { return radius_;}
  void set_radius(float radius);

  void Render(const azer::Matrix4& world, const azer::Matrix4& pv, 
              azer::Renderer* renderer);
  void set_xaxis_color(const azer::Vector4& c) { axis_color_[0] = c;}
  void set_yaxis_color(const azer::Vector4& c) { axis_color_[1] = c;}
  void set_zaxis_color(const azer::Vector4& c) { axis_color_[2] = c;}
  const azer::Vector4& xaxis_color() const { return axis_color_[0];}
  const azer::Vector4& yzxis_color() const { return axis_color_[1];}
  const azer::Vector4& zaxis_color() const { return axis_color_[2];}
  void reset_color();
 private:
  float radius_;
  azer::Matrix4 axis_world_[3];
  azer::Matrix4 world_[3];
  azer::Vector4 axis_color_[3];
  azer::Matrix4 scale_;
  azer::GeometryObjectPtr circle_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(CircleCoordinateObject);
};

class RotationControllerObject {
 public:
  RotationControllerObject();
  ~RotationControllerObject();
  
  void SetSelectedAxis(int32 axis);
  void SetSelectedColor(const azer::Vector4& color);

  const azer::Vector3& position() const { return position_;}
  void SetPosition(azer::Vector3& position);

  float radius() const { return radius_;}
  void set_radius(float r);
  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);
 private:
  azer::GeometryObjectPtr sphere_;
  scoped_ptr<CircleCoordinateObject> circles_;

  int32 selected_axis_;
  float radius_;
  azer::Vector3 position_;
  azer::Vector4 selected_color_;
  azer::Vector4 sphere_color_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(RotationControllerObject);
};
}
