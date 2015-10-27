#pragma once

#include "azer/render/render.h"
#include "lordaeron/render/light.h"

namespace lord {
class GlobalEnvironmentParams : public azer::EffectParamsProvider {
 public:
  GlobalEnvironmentParams();
  ~GlobalEnvironmentParams();

  Light* light() { return light_.get();}

  const azer::Camera* camera() { return camera_;}
  azer::Camera* mutable_camera() { return camera_;}
  const azer::Matrix4& GetProjView() const { return projview_;}

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
