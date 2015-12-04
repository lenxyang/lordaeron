#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/light.h"

namespace lord {
class SpotLightObject {
 public:
  explicit SpotLightObject(Light* light);
  ~SpotLightObject();
 private:
  LightPtr light_;
  DISALLOW_COPY_AND_ASSIGN(SpotLightObject);
};
}  // namespace lord
