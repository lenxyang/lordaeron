#pragma once

#include <vector>
#include "azer/render/render.h"

namespace lord {
class DiffuseEffect;
class Light;
class SceneNode;

class LightMesh : public azer::Mesh {
 public:
  LightMesh(SceneNode* node, DiffuseEffect* desc);
  void Render(azer::Renderer* renderer) override;
 protected:
  void RenderPickedPart(azer::Renderer* renderer);
  SceneNode* node_;
  std::vector<azer::MeshPartPtr> picked_part_;
  scoped_refptr<DiffuseEffect> effect_;
  DISALLOW_COPY_AND_ASSIGN(LightMesh);
};

class PointLightControllerMesh : public LightMesh {
 public:
  PointLightControllerMesh(SceneNode* node, DiffuseEffect* effect);
 private:
  void InitPickedMesh();
  DISALLOW_COPY_AND_ASSIGN(PointLightControllerMesh);
};

class SpotLightControllerMesh : public LightMesh {
 public:
  SpotLightControllerMesh(SceneNode* node, DiffuseEffect* effect);
 private:
  void InitMesh();
  void InitPickedMesh();
  DISALLOW_COPY_AND_ASSIGN(SpotLightControllerMesh);
};

class DirLightControllerMesh : public LightMesh {
 public:
  DirLightControllerMesh(SceneNode* node, DiffuseEffect* effect);
 private:
  void InitMesh();
  void InitPickedMesh();
  DISALLOW_COPY_AND_ASSIGN(DirLightControllerMesh);
};

azer::MeshPtr CreateLightMesh(SceneNode* node);
}  // namespace lord
