#pragma once

#include "azer/render/render.h"

namespace lord {
class SceneNode;
azer::MeshPtr CreatePointLightMesh(SceneNode* node);
azer::MeshPtr CreateSpotLightMesh();
azer::MeshPtr CreateDirectionalLightMesh();
}  // namespace lord
