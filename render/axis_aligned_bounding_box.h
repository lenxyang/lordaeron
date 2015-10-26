#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/render/bounding_volumn.h"

namespace lord {
class AxisAlignedBoundingBox : public BoundingVolumn {
 public:
  AxisAlignedBoundingBox(azer::Vector3 vmin, azer::Vector3 vmax);
  AxisAlignedBoundingBox();

  azer::Vector3* mutable_vmin() { return &vmin_;}
  azer::Vector3* mutable_vmax() { return &vmax_;}
  const azer::Vector3& vmin() const { return vmin_;}
  const azer::Vector3& vmax() const { return vmax_;}

  void SetColor(const azer::Vector4& color) {color_ = color;}
  void Update(const azer::Camera& camera, 
              const azer::TransformHolder& holder) override;
  void Render(azer::Renderer* renderer) override;
 private:
  azer::Vector3 vmin_, vmax_;
  azer::GeometryObjectPtr object_;
  azer::Matrix4 world_;
  azer::Matrix4 pvw_;
  azer::Vector4 color_;
  azer::EffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(AxisAlignedBoundingBox);
};
}  // namespace lord
