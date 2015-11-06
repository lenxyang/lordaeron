#include "lordaeron/render/light.h"

#include "base/logging.h"
#include "lordaeron/render/light_mesh.h"

namespace lord {
using namespace azer;
Light::Light(const DirLight& light)
    : type_(kDirectionalLight),
      dir_light_(light) {
}

Light::Light(const PointLight& light) 
    : type_(kPointLight),
      point_light_(light) {
  mesh_ = CreatePointLightMesh();
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

const SpotLight& Light::spot_light() const {
  CHECK(type() == kSpotLight);
  return spot_light_;
}

SpotLight* Light::mutable_spot_light() {
  CHECK(type() == kSpotLight);
  return &spot_light_;
}

namespace {
const Vector4 none_color(0.0f, 0.0f, 0.0f, 0.0f);
}
const Vector4& Light::diffuse() const {
  switch (type()) {
    case kDirectionalLight:
      return dir_light_.diffuse;
    case kPointLight:
      return point_light_.diffuse;
    case kSpotLight:
      return spot_light_.diffuse;
    default:
      CHECK(false);
      return none_color;
  }
}

const Vector4& Light::ambient() const {
  switch (type()) {
    case kDirectionalLight:
      return dir_light_.ambient;
    case kPointLight:
      return point_light_.ambient;
    case kSpotLight:
      return spot_light_.ambient;
    default:
      CHECK(false);
      return none_color;
  }
}

const Vector4& Light::specular() const {
  switch (type()) {
    case kDirectionalLight:
      return dir_light_.specular;
    case kPointLight:
      return point_light_.specular;
    case kSpotLight:
      return spot_light_.specular;
    default:
      CHECK(false);
      return none_color;
  }
}
}  // namespace lord
