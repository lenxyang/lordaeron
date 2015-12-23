#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/effect/material.h"

namespace lord {
enum {
  kResTypeNone,
  kResTypeMesh,
  kResTypeMaterial,
  kResTypeEffect,
  kResTypeLight,
  kResTypeScene,
  kResTypeVertexDesc,
};

struct Resource {
  azer::MeshPtr mesh;
  azer::EffectPtr effect;
  azer::VertexDescPtr vertex_desc;
  MaterialPtr material;
  LightPtr light;
  SceneNodePtr scene;
  int32 type;
  int32 retcode;

  Resource() : type(kResTypeNone), retcode(-1) {}
};

int32 GetTypeFromString(const std::string& str);
}  // namespace lord
