#pragma once

#include "azer/render/render.h"

namespace lord {

azer::Vector4 CalcWorldPosFromScreen(const gfx::Point& pt, const gfx::Size& size, 
                                     const azer::Camera* camera);

azer::Ray GetPickingRay(const gfx::Point& pt, const gfx::Size& size,
                        const azer::Camera* camera);
}  // namespace lord
