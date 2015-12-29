#include "lordaeron/effect/light.h"

#include "base/logging.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using namespace azer;
Light::Light(const DirLight& light)
    : type_(kDirectionalLight),
      dir_light_(light) {
}

Light::Light(const PointLight& light) 
    : type_(kPointLight),
      point_light_(light) {
}

Light::Light(const SpotLight& light)
    : type_(kSpotLight),
      spot_light_(light) {
}

void Light::SetDirLight(const DirLight& l) {
  type_ = kDirectionalLight;
  dir_light_ = l;
}

void Light::SetPointLight(const PointLight& l) {
  type_ = kPointLight;
  point_light_ = l;
}

void Light::SetSpotLight(const SpotLight& l) {
  type_ = kSpotLight;
  spot_light_ = l;
}

const DirLight& Light::dir_light() const {
  CHECK(type() == kDirectionalLight);
  return dir_light_;
}

const PointLight& Light::point_light() const {
  CHECK(type() == kPointLight);
  return point_light_;
}

const SpotLight& Light::spot_light() const {
  CHECK(type() == kSpotLight);
  return spot_light_;
}

namespace {
const Vector4 none_color(0.0f, 0.0f, 0.0f, 0.0f);
const Vector3 none_dir(0.0f, 0.0f, 0.0f);
}

void Light::set_enable(bool enable) {
  float value = enable ? 1.0f : 0.0f;
  switch (type()) {
    case kDirectionalLight:
      dir_light_.enable = value;
      break;
    case kPointLight:
      point_light_.enable = value;
      break;
    case kSpotLight:
      spot_light_.enable = value;
      break;
    default:
      CHECK(false);
  }
}

void Light::set_diffuse(const azer::Vector4& color) {
  switch (type()) {
    case kDirectionalLight:
      dir_light_.diffuse = color;
      break;
    case kPointLight:
      point_light_.diffuse = color;
      break;
    case kSpotLight:
      spot_light_.diffuse = color;
      break;
    default:
      CHECK(false);
  }
}

void Light::set_ambient(const azer::Vector4& color) {
  switch (type()) {
    case kDirectionalLight:
      dir_light_.ambient = color;
      break;
    case kPointLight:
      point_light_.ambient = color;
      break;
    case kSpotLight:
      spot_light_.ambient = color;
      break;
    default:
      CHECK(false);
  }
}

void Light::set_specular(const azer::Vector4& color) {
  switch (type()) {
    case kDirectionalLight:
      dir_light_.specular = color;
      break;
    case kPointLight:
      point_light_.specular = color;
      break;
    case kSpotLight:
      spot_light_.specular = color;
      break;
    default:
      CHECK(false);
  }
}

void Light::set_position(const azer::Vector3& pos) {
  switch (type()) {
    case kDirectionalLight:
      CHECK(false) << "DirLight has no position";
      break;
    case kPointLight:
      point_light_.position = pos;
      
      break;
    case kSpotLight:
      spot_light_.position = pos;
      break;
    default:
      CHECK(false);
  }
}

void Light::set_directional(const azer::Vector3& dir) {
  switch (type()) {
    case kDirectionalLight:
      dir_light_.direction = dir;
      break;
    case kPointLight:
      CHECK(false) << "Point Light has no dir";
      break;
    case kSpotLight:
      spot_light_.direction = dir;
      break;
    default:
      CHECK(false);
  }
}

bool Light::enable() const {
  float v = 0.0f;
  switch (type()) {
    case kDirectionalLight:
      v = dir_light_.enable;
      break;
    case kPointLight:
      v = point_light_.enable;
      break;
    case kSpotLight:
      v = spot_light_.enable;
      break;
    default:
      CHECK(false);
      return false;
  }
  return std::abs(v - 1.0f) < 0.00001f;
}

const azer::Vector3& Light::directional() const {
  switch (type()) {
    case kDirectionalLight:
      return dir_light_.direction;
    case kPointLight:
      CHECK(false) << " no directional for Point Light";
      return none_dir;
    case kSpotLight:
      return spot_light_.direction;
    default:
      CHECK(false);
      return none_dir;
  }
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

void Light::AddObserver(LightObserver* observer) {
  observers_.AddObserver(observer);
}

void Light::RemoveObserver(LightObserver* observer) {
  observers_.RemoveObserver(observer);
}

bool Light::HasObserver(LightObserver* observer) const {
  return observers_.HasObserver(observer);
}

void Light::InitShadowmapRenderer(const gfx::Size& size) {
  switch (type()) {
    case kSpotLight:
      InitSpotLightShadowmapRenderer(size);
      break;
    default:
      CHECK(false);
      break;
  }
}

void Light::InitSpotLightShadowmapRenderer(const gfx::Size& size) {
  RenderSystem* rs = RenderSystem::Current();
  Texture::Options opt;
  opt.target = (Texture::BindTarget)
      (Texture::kShaderResource | Texture::kRenderTarget);
  opt.size = size;
  Viewport viewport(0, 0, opt.size.width(), opt.size.height());
  shadowmap_renderer_ = rs->CreateRenderer(opt);
  shadowmap_renderer_->SetViewport(viewport);
  shadowmap_ = shadowmap_renderer_->GetRenderTarget(0)->GetTexture();
}

void InitShadowMapCamera(const Light* light, Camera* camera) {
  DCHECK_EQ(light->type(), kSpotLight);
  Radians rad(std::acos(light->spot_light().phi));
  camera->mutable_frustum()->set_aspect(rad);
  camera->mutable_frustum()->set_fovy(1.0f);
}
}  // namespace lord
