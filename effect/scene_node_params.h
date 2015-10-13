#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
class SceneNodeParams : public azer::EffectParamsProvider {
 public:
  SceneNodeParams();
  ~SceneNodeParams();

  const azer::Matrix4& world() const { return world_;}
  const std::vector<LightPtr>& light() { return light_;}

  void UpdateParams(const FrameArgs& args) override;
 private:
  std::vector<LightPtr> light_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeParams);
};
}  // namespace lord
