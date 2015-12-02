#pragma once

#include "azer/render/render.h"

namespace lord {

class SceneNode;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class SceneBVParamsAdapter : public azer::EffectParamsAdapter {
 public:
  static const azer::EffectAdapterKey kAdapterKey;
  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* effect,
             const azer::EffectParamsProvider* params) const override;
};

class SceneBVProvider : public azer::EffectParamsProvider {
 public:
  SceneBVProvider(SceneNode* node);
  ~SceneBVProvider();
  
  // override from azer::EffectParamsProvider
  void UpdateParams(const azer::FrameArgs& args) override;
 private:
  friend class SceneBVParamsAdapter;
  SceneNode* node_;
  azer::Vector4 color_;
  azer::Matrix4 world_;
  DISALLOW_COPY_AND_ASSIGN(SceneBVProvider);
};

azer::MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node);
}  // namespace lord
