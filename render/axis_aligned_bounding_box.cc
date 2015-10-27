#include "lordaeron/render/axis_aligned_bounding_box.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
using namespace azer;
AxisAlignedBoundingBox::AxisAlignedBoundingBox(Vector3 vmin, Vector3 vmax) 
    : BoundingVolumn(kAxisAlignedBoundingBox), 
      vmin_(vmin), 
      vmax_(vmax) {
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox()
    : BoundingVolumn(kAxisAlignedBoundingBox), 
      vmin_(Vector3( 99999.0f,  99999.0f,  99999.0f)),
      vmax_(Vector3(-99999.0f, -99999.0f, -99999.0f)) {
}
}  // namespace lord
