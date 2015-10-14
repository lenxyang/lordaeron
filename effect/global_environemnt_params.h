#pragma once

#include "azer/render/render.h"

namespace lord {
class GlobalEnvironmentParams : public azer::EffectParamsProvider {
 public:
  GlobalEnvironmentParams();
  ~GlobalEnvironmentParams();

  const azer::Matrix4& GetProjView() const { return projview_;}
  void SetCamera(azer::Camera* camera) { camera_ = camera;}

  void UpdateParams(const azer::FrameArgs& args) override;
 private:
  azer::Camera* camera_;
  azer::Matrix4 projview_;
  DISALLOW_COPY_AND_ASSIGN(GlobalEnvironmentParams);
};
}  // namespace lord
