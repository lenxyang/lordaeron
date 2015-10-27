#include "lordaeron/render/bounding_volumn.h"

#include "azer/math/math.h"

namespace lord {
BoundingVolumn::BoundingVolumn(int32 type)
    : type_(type) {
}

BoundingVolumn::~BoundingVolumn() {
}

void UpdateVMinAndVMax(const azer::Vector3 pos, azer::Vector3* vmin,
                       azer::Vector3* vmax) {
  if (pos.x < vmin->x) vmin->x = pos.x;
  if (pos.y < vmin->y) vmin->y = pos.y;
  if (pos.z < vmin->z) vmin->z = pos.z;
  if (pos.x > vmax->x) vmax->x = pos.x;
  if (pos.y > vmax->y) vmax->y = pos.y;
  if (pos.z > vmax->z) vmax->z = pos.z;
}
}  // namespace lord
