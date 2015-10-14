#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/scene_node_params.h"

namespace lord {
class SceneMesh : public azer::Mesh {
 public:
  SceneMesh(SceneNode* node);
  ~SceneMesh();

 private:
  SceneNodeParamsPtr node_params_;
  SceneNode* node_;
  DISALLOW_COPY_AND_ASSIGN(SceneMesh);
};

typedef scoped_refptr<SceneMesh> SceneMeshPtr; 
}  // namespace lord
