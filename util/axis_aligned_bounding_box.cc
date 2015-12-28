#include "lordaeron/util/axis_aligned_bounding_box.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/env.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
using namespace azer;

namespace {
const int kIndices[] = {0, 2, 1, 0, 3, 2,  // front
                        1, 6, 5, 1, 2, 6,  // right
                        5, 7, 4, 5, 6, 7,  // back
                        4, 3, 0, 4, 7, 3,  // left
                        4, 1, 5, 4, 0, 1,  // top
                        3, 6, 2, 3, 7, 6}; // bottom
};

AxisAlignedBoundingBox::AxisAlignedBoundingBox(Vector3 vmin, Vector3 vmax) 
    : vmin_(vmin), 
      vmax_(vmax) {
  Vector3 pt[8]; 
  pt[0] = Vector3(vmin_.x, vmax_.y,  vmax_.z);
  pt[1] = Vector3(vmax_.x, vmax_.y,  vmax_.z);
  pt[2] = Vector3(vmax_.x, vmin_.y,  vmax_.z);
  pt[3] = Vector3(vmin_.x, vmin_.y,  vmax_.z);
  pt[4] = Vector3(vmin_.x, vmax_.y,  vmin_.z);
  pt[5] = Vector3(vmax_.x, vmax_.y,  vmin_.z);
  pt[6] = Vector3(vmax_.x, vmin_.y,  vmin_.z);
  pt[7] = Vector3(vmin_.x, vmin_.y,  vmin_.z);
  int index = 0;
  for (uint32 i = 0; i < arraysize(kIndices); i+=3, ++index) {
    int32 idx1 = kIndices[i];
    int32 idx2 = kIndices[i+1];
    int32 idx3 = kIndices[i+2];
    subplane_[index].reset(pt[idx1], pt[idx2], pt[idx3]);
  }
}

bool AxisAlignedBoundingBox::IsIntersect(const azer::Ray& ray) const {
  const TrianglePlane* plane = subplane_;
  for (; plane < subplane_ + arraysize(subplane_); plane++) {
    Vector3 pt = plane->intersect(ray);
    if (plane->IsPointIn(pt)) {
      return true;
    }
  }
  return false;
}
}  // namespace lord
