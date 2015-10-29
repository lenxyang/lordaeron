#pragma once

#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/size.h"
#include "azer/render/render.h"

namespace lord {
azer::Vector4 CalcClickedWorldPos(const gfx::Point& pt, const gfx::Size& size,
                                  azer::Camera* camera);

azer::Ray GetPickingRay(const gfx::Point& pt, const gfx::Size& size,
                            azer::Camera* camera);
}  // namespace lord
