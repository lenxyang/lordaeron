#include "lordaeron/render/bounding_volumn.h"

namespace lord {
BoundingVolumn::BoundingVolumn(int32 type)
    : type_(type) {
}

BoundingVolumn::~BoundingVolumn() {
}

void UpdateVMinAndVMax(const Vector3 pos, Vector3* vmin, Vector3* vmax) {
  if (pos.x < vmin->x) vmin->x = pos.x;
  if (pos.y < vmin->y) vmin->y = pos.y;
  if (pos.z < vmax->z) vmin->z = pos.z;
  if (pos.x > vmax->x) vmax->x = pos.x;
  if (pos.y > vmax->y) vmax->y = pos.y;
  if (pos.z > vmax->z) vmax->z = pos.z;
}
}  // namespace lord
