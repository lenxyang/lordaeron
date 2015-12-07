#pragma once

#include <vector>
#include "azer/render/render.h"

namespace lord {
class DiffuseEffect;
class Light;
class SceneNode;
typedef scoped_refptr<Light> LightPtr;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class LightMesh : public azer::Mesh {
 public:
  LightMesh(SceneNode* node, DiffuseEffect* desc);
  void Render(azer::Renderer* renderer) override;

  const azer::Matrix4& local_transform() const { return local_transform_;}
 protected:
  void RenderPickedPart(azer::Renderer* renderer);
  SceneNode* node_;
  std::vector<azer::MeshPartPtr> picked_part_;
  scoped_refptr<DiffuseEffect> effect_;
  azer::Matrix4 local_transform_;
  DISALLOW_COPY_AND_ASSIGN(LightMesh);
};
typedef scoped_refptr<LightMesh> LightMeshPtr;

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

class LightMeshProvider : public azer::EffectParamsProvider {
 public:
  LightMeshProvider(SceneNode* node, const azer::Matrix4& local_transform);
  ~LightMeshProvider();
  void UpdateParams(const azer::FrameArgs& args) override;
  const azer::Matrix4& world() const;
  const azer::Vector4& color() const;
  const azer::Vector4& emission() const;
 private:
  SceneNodePtr node_;
  const Light* light_;
  azer::Matrix4 local_transform_;
  azer::Matrix4 world_;
  DISALLOW_COPY_AND_ASSIGN(LightMeshProvider);
};

class LightMeshDiffuseEffectAdapter : public azer::EffectParamsAdapter {
 public:
  LightMeshDiffuseEffectAdapter();
  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* e, const azer::EffectParamsProvider* params)
      const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LightMeshDiffuseEffectAdapter);
};

LightMeshPtr CreateLightMesh(SceneNode* node);
}  // namespace lord
