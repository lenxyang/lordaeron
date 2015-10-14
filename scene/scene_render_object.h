#pragma once

#include "azer/render/render.h"

namespace lord {

class SceneRenderObject {
 public:
  SceneRenderObject();
  ~SceneRenderObject();

  SceneNode* node() { return node_;}
 private:
  SceneNode* node_;
  std::vector<MeshObject> objects_;
  std::vector<azer::EffectPtr> effects_;
  std::vector<azer::EffectParamsProvider*> provider_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderObject);
};
}  // namespace lord
