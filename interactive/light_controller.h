#pragma once

#include "azer/render/render.h"
#include "lordaeron/interactive/picking_controller.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
class SceneNode;
class PointLightObject : public PickingObject {
 public:
  PointLightObject(const azer::Camera* camera, SceneNode* node);
  ~PointLightObject();

  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  // to draw center and range ball
  const azer::Camera* camera_;
  SceneNode* node_;
  
  DiffuseEffectPtr effect_;
  azer::MeshPartPtr sphere_;
  azer::Matrix4 world_;
  azer::Matrix4 pv_;
  DISALLOW_COPY_AND_ASSIGN(PointLightObject);
};

}  // namespace lord
