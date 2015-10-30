#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/util/bounding_volumn.h"

namespace lord {
class AxisAlignedBoundingBox : public BoundingVolumn {
 public:
  AxisAlignedBoundingBox(azer::Vector3 vmin, azer::Vector3 vmax);

  bool IsIntersect(const azer::Ray& ray) const override;

  const azer::Vector3& vmin() const { return vmin_;}
  const azer::Vector3& vmax() const { return vmax_;}
 private:
  azer::Vector3 vmin_, vmax_;
  azer::TrianglePlane subplane_[12];
  DISALLOW_COPY_AND_ASSIGN(AxisAlignedBoundingBox);
};
}  // namespace lord
