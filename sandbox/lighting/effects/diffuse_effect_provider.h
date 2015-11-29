#pragma once

#include "azer/math/math.h"
#include "azer/render/render.h"

namespace lord {
namespace sandbox {
class DiffuseEffectProvider : public azer::EffectParamsProvider {
 public:
  DiffuseEffectProvider();
  ~DiffuseEffectProvider();

  azer::Vector4 color() const { return color_;}
  void SetColor(const azer::Vector4& color) { color_ = color;}
  
  void UpdateParams(const azer::FrameArgs& args) override;
 private:
  azer::Vector4 color_;
  DISALLOW_COPY_AND_ASSIGN(DiffuseEffectProvider);
};
}  // namespace sandbox
}  // namespace lord
