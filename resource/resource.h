#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
enum {
  kResTypeNone,
  kResTypeMesh,
  kResTypeMaterial,
  kResTypeEffect,
  kResTypeLight,
  kResTypeScene,
};

struct Resource {
  azer::MeshPtr mesh;
  azer::EffectParamsProviderPtr material;
  azer::EffectPtr effect;
  LightPtr light;
  SceneNodePtr scene;
  int32 type;
  int32 retcode;

  Resource() : type(kResTypeNone), retcode(-1) {}
};
}  // namespace lord
