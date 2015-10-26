#pragma once

#include "base/basictypes.h"
#include "azer/render/render.h"

namespace lord {

class BoundingVolumn {
 public:
  enum {
    kAxisAlignedBoundingBox,
    kOrientationBoundingBox,
    kBoundingSphere,
  };

  BoundingVolumn(int32 type);
  ~BoundingVolumn();

  int32 type() const { return type_;}
  virtual void Update(const azer::Camera& camera, 
                      const azer::TransformHolder& holder) = 0;
  virtual void Render(azer::Renderer* renderer) = 0;
 private:
  const int32 type_;
  DISALLOW_COPY_AND_ASSIGN(BoundingVolumn);
};

void UpdateVMinAndVMax(const Vector3 pos, Vector3* vmin, Vector3* vmax);
}  // namespace lord
