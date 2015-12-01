#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
class PointLightObject {
 public:
  PointLightObject();
  ~PointLightObject();

  void set_range_color(const azer::Vector4& col) { range_color_ = col;}
  void set_light_color(const azer::Vector4& col) { color_ = col;}
  void SetPosition(const azer::Vector3& pos) { position_ = pos;}
  void SetRange(float range);
  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);
 private:
  // to draw center and range ball
  float range_;
  azer::Vector3 position_;
  azer::Vector4 range_color_;
  azer::Vector4 color_;
  azer::EntityPtr sphere_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(PointLightObject);
};

PointLightObject* GetPointLightObjectPointer();
}  // namespace lord
