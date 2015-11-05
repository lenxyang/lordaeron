#pragma once

#include "azer/render/render.h"

namespace lord {
class AxisObject;

class DirectionalLightObject {
 public:
  DirectionalLightObject();
  ~DirectionalLightObject();

  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);
 private:
  azer::Matrix4 rotation_;
  scoped_ptr<AxisObject> axis_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(DirectionalLightObject);
};
}  // namespace lord
