#pragma once

#include "azer/render/render.h"

namespace lord {
class SceneNode;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class SceneBoundingBox : public azer::EffectParamsProvider,
                         public azer::Mesh {
 public:
  SceneBoundingBox(SceneNode* node);
  ~SceneBoundingBox();
  
  // override from azer::EffectParamsProvider
  void UpdateParams(const azer::FrameArgs& args) override;

  // override from azer::Mesh
  void UpdateProviderParams(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer, azer::Effect* effect) override;
 private:
  class ParamsAdapter : public azer::EffectParamsAdapter {
   public:
    azer::EffectAdapterKey key() const override;
    void Apply(azer::Effect* effect, const azer::EffectParamsProvider* params) const override;
  };

  friend class ParamsAdapter;
  SceneNode* node_;
  azer::Vector4 color_;
  azer::Matrix4 world_;
  azer::Matrix4 pv_;
  azer::EffectPtr effect_;
  azer::RenderClosurePtr frame_;
  azer::RenderClosurePtr object_;
  DISALLOW_COPY_AND_ASSIGN(SceneBoundingBox);
};
}  // namespace lord
