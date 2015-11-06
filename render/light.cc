#include "lordaeron/render/light.h"

#include "base/logging.h"

namespace lord {
Light::Light(const DirLight& light)
    : type_(kDirectionalLight),
      dir_light_(light) {
}

Light::Light(const PointLight& light) 
    : type_(kPointLight),
      point_light_(light) {
}

const DirLight& Light::dir_light() const {
  CHECK(type() == kDirectionalLight);
  return dir_light_;
}

DirLight* Light::mutable_dir_light() {
  CHECK(type() == kDirectionalLight);
  return &dir_light_;
}

const PointLight& Light::point_light() const {
  CHECK(type() == kPointLight);
  return point_light_;
}

PointLight* Light::mutable_point_light() {
  CHECK(type() == kPointLight);
  return &point_light_;
}
}  // namespace lord
