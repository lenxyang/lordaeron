#include "lordaeron/render/light.h"

#include "base/logging.h"
#include "lordaeron/render/light_mesh.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using namespace azer;
Light::Light(const DirLight& light)
    : type_(kDirectionalLight),
      dir_light_(light) {
  mesh_ = CreateDirectionalLightMesh();
}

Light::Light(const PointLight& light) 
    : type_(kPointLight),
      point_light_(light) {
  mesh_ = CreatePointLightMesh();
}

Light::Light(const SpotLight& light)
    : type_(kSpotLight),
      spot_light_(light) {
  mesh_ = CreateSpotLightMesh();
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

void Light::OnSceneNodeLocationChanged(SceneNode* node,
                                       const azer::Vector3& prevpos) {
  switch (type()) {
    case kDirectionalLight:
      return;
    case kSpotLight:
      spot_light_.position = node->position();
      return;
    case kPointLight:
      point_light_.position = node->position();
      return;
    default:
      CHECK(false);
      return;
  }
}

void Light::OnSceneNodeOrientationChanged(SceneNode* node, 
                                          const azer::Quaternion& prev_orient) {
  switch (type()) {
    case kDirectionalLight:
      dir_light_.direction = node->orientation().zaxis();
      return;
    case kSpotLight:
      spot_light_.direction = node->orientation().zaxis();
      return;
    case kPointLight:
      return;
    default:
      CHECK(false);
      return;
  }
}
}  // namespace lord
