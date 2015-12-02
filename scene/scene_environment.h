#pragma once

#include <deque>
#include <vector>

#include "lordaeron/render/light.h"

namespace lord {
class SceneEnvironment : public azer::EffectParamsProvider {
 public:
  SceneEnvironment();
  ~SceneEnvironment();

  void reset();

  void PushLight(LightPtr light);
  void PopLight();

  const Lights light() const { return light_;}
  void UpdateParams(const azer::FrameArgs& args) override;
 private:
  Lights light_;
  DISALLOW_COPY_AND_ASSIGN(SceneEnvironment);
};
}  // namespace lord
