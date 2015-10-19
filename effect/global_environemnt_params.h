#pragma once

#include "azer/render/render.h"
#include "lordaeron/render/light.h"

namespace lord {
class GlobalEnvironmentParams : public azer::EffectParamsProvider {
 public:
  GlobalEnvironmentParams();
  ~GlobalEnvironmentParams();

  const azer::Matrix4& GetProjView() const { return projview_;}
  LightPtr light() { return light_;}
  void SetCamera(azer::Camera* camera) { camera_ = camera;}
  void SetLight(LightPtr light) { light_ = light;}

  void UpdateParams(const azer::FrameArgs& args) override;
 private:
  azer::Camera* camera_;
  azer::Matrix4 projview_;
  LightPtr light_;
  DISALLOW_COPY_AND_ASSIGN(GlobalEnvironmentParams);
};
}  // namespace lord
