#pragma once

#include "azer/render/render.h"
#include "lordaeron/interactive/picking_controller.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
class SceneNode;
class PointLightObject : public PickingObject,
                         public azer::EffectParamsProvider {
 public:
  explicit PointLightObject(SceneNode* node);
  ~PointLightObject() override;

  void UpdateParams(const azer::FrameArgs& args) override;
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  // to draw center and range ball
  LightPtr light_;
  SceneNode* node_;
  azer::EntityPtr sphere_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(PointLightObject);
};

}  // namespace lord
