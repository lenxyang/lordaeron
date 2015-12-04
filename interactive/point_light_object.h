#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
class PointLightObject {
 public:
  PointLightObject();
  ~PointLightObject();

  void SetLightObject(Lighting* light);
  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);
 private:
  // to draw center and range ball
  LightPtr light_;
  azer::EntityPtr sphere_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(PointLightObject);
};

PointLightObject* GetPointLightObjectPointer();
}  // namespace lord
