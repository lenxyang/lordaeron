#include "lordaeron/render/light.h"

#include "base/logging.h"

namespace lord {
Light::Light(const DirLight& light)
    : type_(kDirectionalLight),
      dir_light_(light) {
}

const DirLight& Light::dir_light() const {
  CHECK(type() == kDirectionalLight);
  return dir_light_;
}
}  // namespace lord
