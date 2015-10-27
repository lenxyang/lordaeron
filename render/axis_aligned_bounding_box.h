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
 private:
  azer::Vector3 vmin_, vmax_;
  DISALLOW_COPY_AND_ASSIGN(AxisAlignedBoundingBox);
};
}  // namespace lord
