#pragma once

#include "azer/render/render.h"

namespace lord {

class SceneNode;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class ParamsAdapter : public azer::EffectParamsAdapter {
 public:
  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* effect, const azer::EffectParamsProvider* params) const override;
};

class ScenVBProvider : public azer::EffectParamsProvider {
 public:
  ScenVBProvider(SceneNode* node);
  ~ScenVBProvider();
  
  // override from azer::EffectParamsProvider
  void UpdateParams(const azer::FrameArgs& args) override;

 private:
  friend class ParamsAdapter;
  SceneNode* node_;
  azer::Vector4 color_;
  azer::Matrix4 world_;
  azer::Matrix4 pv_;
  DISALLOW_COPY_AND_ASSIGN(ScenVBProvider);
};
}  // namespace lord
