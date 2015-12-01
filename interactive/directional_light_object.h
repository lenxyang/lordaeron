#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/render/light.h"

namespace lord {
class AxisObject;

class DirectionalLightObject {
 public:
  DirectionalLightObject(LightPtr light);
  ~DirectionalLightObject();

  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);
 private:
  scoped_ptr<AxisObject> axis_;
  DiffuseEffectPtr effect_;
  LightPtr light_;
  DISALLOW_COPY_AND_ASSIGN(DirectionalLightObject);
};
}  // namespace lord
