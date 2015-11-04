#pragma once

#include <vector>
#include "azer/render/render.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
class ControllerObject {
 public:
  ControllerObject(DiffuseEffect* effect);
  ~ControllerObject();

  void SetPosition(const azer::Vector3& pos);
  void SetPV(const azer::Matrix4& pv) { pv_ = pv;}
  virtual void Render(azer::Renderer* renderer) = 0;
 private:
  azer::Matrix4 pv_;
  azer::Matrix4 world_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(ControllerObject);
};

azer::EntityPtr CreatePlane(const azer::Vector4 pos[4], azer::VertexDesc* desc);

azer::EntityPtr CreateLineList(const azer::Vector4* pos, int32 count, azer::VertexDesc* desc);
}  // namespace lord
