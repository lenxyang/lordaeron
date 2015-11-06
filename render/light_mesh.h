#pragma once

#include "azer/render/render.h"
#include "lordaeron/render/light.h"

namespace lord {
class SceneNode;
azer::MeshPtr CreatePointLightMesh();
azer::MeshPtr CreateSpotLightMesh();
azer::MeshPtr CreateDirectionalLightMesh();

class LightColorProvider : public azer::EffectParamsProvider {
 public:
  LightColorProvider(Light* light);
  ~LightColorProvider();
  void UpdateParams(const azer::FrameArgs& args) override;
  const azer::Vector4& color() const;
 private:
  LightPtr light_;
  DISALLOW_COPY_AND_ASSIGN(LightColorProvider);
};

class LightColorDiffuseEffectAdapter : public azer::EffectParamsAdapter {
 public:
  LightColorDiffuseEffectAdapter();

  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* e, const azer::EffectParamsProvider* params) 
      const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LightColorDiffuseEffectAdapter);
};
}  // namespace lord
