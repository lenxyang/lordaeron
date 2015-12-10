#pragma once

#include "azer/math/math.h"
#include "third_party/skia/include/core/SkColor.h"

namespace lord {
SkColor SkColorFromVector4(const azer::Vector4& color) {
  return SkColorSetARGB(color.w * 255.0f, color.x * 255.0f, color.y * 255.0f,
                        color.z * 255.0f);
                        
}

SkColor SkColorFromVector3(const azer::Vector3& color) {
  return SkColorSetRGB(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f);
}

azer::Vector4 Vector4FromSkColor(SkColor c) {
  float a = static_cast<float>(SkColorGetA(c));
  float r = static_cast<float>(SkColorGetR(c));
  float g = static_cast<float>(SkColorGetG(c));
  float b = static_cast<float>(SkColorGetB(c));
  return azer::Vector4(r, g, b, a);
}

azer::Vector3 Vector3FromSkColor(SkColor c) {
  float r = static_cast<float>(SkColorGetR(c)) / 255.0f;
  float g = static_cast<float>(SkColorGetG(c)) / 255.0f;
  float b = static_cast<float>(SkColorGetB(c)) / 255.0f;
  return azer::Vector3(r, g, b);
}
}
