#pragma once

#include "azer/render/render.h"

namespace lord {

azer::Vector4 CalcWorldPosFromScreen(const gfx::Point& pt, const gfx::Size& size, 
                                     const azer::Camera* camera);

azer::Ray GetPickingRay(const gfx::Point& pt, const gfx::Size& size,
                        const azer::Camera* camera);

void PickingPlane(const azer::Ray& ray, const azer::Plane& plane, 
                  azer::Vector3* pt, bool* parallel);
}  // namespace lord
