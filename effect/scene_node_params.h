#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/render/light.h"

namespace lord {
class SceneNodeParams : public azer::EffectParamsProvider {
 public:
  SceneNodeParams(SceneNode* node);
  ~SceneNodeParams();

  const azer::Matrix4& world() const { return world_;}
  const std::vector<LightPtr>& light() { return light_;}

  void UpdateParams(const azer::FrameArgs& args) override;
 private:
  std::vector<LightPtr> light_;
  azer::Matrix4 world_;
  SceneNode* node_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeParams);
};

typedef scoped_refptr<SceneNodeParams> SceneNodeParamsPtr;
}  // namespace lord
