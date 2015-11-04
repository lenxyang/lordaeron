#pragma once

#include "azer/render/render.h"

namespace lord {
azer::EntityPtr CreatePlane(const azer::Vector4 pos[4], azer::VertexDesc* desc);

azer::EntityPtr CreateLineList(const azer::Vector4* pos, int32 count, azer::VertexDesc* desc);
}  // namespace lord
