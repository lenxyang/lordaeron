#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
class XYZAxisObject {
 public:
  XYZAxisObject(DiffuseEffect* effect);

  const azer::Vector3& position() const { return position_;}
  void SetPosition(const azer::Vector3& pos);
  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);

  void set_length(float length);
  void set_color(const azer::Vector4& col, int32 index);
  void reset_color();

  int32 Picking(const azer::Ray& ray, const azer::Camera& camera);
 private:
  azer::Vector4 color_[3];
  azer::Matrix4 rotation_[3];
  azer::Vector3 position_;
  azer::MeshPartPtr axis_;
  azer::DepthStencilStatePtr depth_state_;
  DiffuseEffectPtr effect_;
  float length_;
  DISALLOW_COPY_AND_ASSIGN(XYZAxisObject);
};
}  // namespace lord
