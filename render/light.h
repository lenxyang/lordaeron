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

enum LightType {
  kDirectionalLight,
};

class Light : public ::base::RefCounted<Light> {
 public:
  explicit Light(const DirLight& light);
  LightType type() const { return type_;}

  const DirLight& dir_light() const;
 private:
  LightType type_;
  DirLight dir_light_;
  DISALLOW_COPY_AND_ASSIGN(Light);
};

typedef scoped_refptr<Light> LightPtr;
}  // namespace lord
