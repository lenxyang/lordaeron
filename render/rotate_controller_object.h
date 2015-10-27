#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"

#include "lordaeron/effect/diffuse_effect.h"

namespace lord {

class CircleCoordinateObject {
 public:
  CircleCoordinateObject(DiffuseEffectPtr effect);
  ~CircleCoordinateObject();

  float radius() const { return radius_;}
  void set_radius(float radius);

  void Render(const azer::Matrix4& world, const azer::Matrix4& pv, 
              azer::Renderer* renderer);
  void xaxis_color(const azer::Vector4& c) { axis_color_[0] = c;}
  void yzxis_color(const azer::Vector4& c) { axis_color_[1] = c;}
  void zaxis_color(const azer::Vector4& c) { axis_color_[2] = c;}
  const azer::Vector4& xaxis_color() const { return axis_color_[0];}
  const azer::Vector4& yzxis_color() const { return axis_color_[1];}
  const azer::Vector4& zaxis_color() const { return axis_color_[2];}
  void reset_color();
 private:
  float radius_;
  azer::Matrix4 axis_world_[3];
  azer::Vector4 axis_color_[3];
  azer::Matrix4 scale_;
  azer::GeometryObjectPtr circle_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(CircleCoordinateObject);
};

class RotateControllerObject {
 public:
  RotateControllerObject();
  ~RotateControllerObject();

  void SetSelectedColor(const azer::Vector4& color);

  void Render(const azer::Matrix4& world, const azer::Matrix4& pv, 
              azer::Renderer* renderer);
 private:
  azer::GeometryObjectPtr sphere_;
  scoped_ptr<CircleCoordinateObject> circles_;
  
  azer::Vector4 selected_color_;
  azer::Vector4 sphere_color_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(RotateControllerObject);
};
}  // namepsace lord
