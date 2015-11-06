#pragma once

#include "base/memory/ref_counted.h"
#include "azer/math/math.h"
#include "azer/render/render.h"

namespace lord {
struct DirLight {
  azer::Vector4 dir;
  azer::Vector4 diffuse;
  azer::Vector4 ambient;
};

struct PointLight {
  azer::Vector3 position;
  float range;
  azer::Vector4 diffuse;
  azer::Vector4 ambient;
  azer::Vector4 specular;
};

enum LightType {
  kDirectionalLight,
  kPointLight,
  kSpotLight,
};

class Light : public ::base::RefCounted<Light> {
 public:
  explicit Light(const DirLight& light);
  explicit Light(const PointLight& light);
  LightType type() const { return type_;}

  const DirLight& dir_light() const;
  DirLight* mutable_dir_light();

  PointLight& point_light() const;
  PointLight* mutable_point_light();
 private:
  LightType type_;
  DirLight dir_light_;
  PointLight point_light_;
  DISALLOW_COPY_AND_ASSIGN(Light);
};

typedef scoped_refptr<Light> LightPtr;
}  // namespace lord
