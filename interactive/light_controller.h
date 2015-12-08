#pragma once

#include <vector>
#include "azer/render/render.h"

namespace lord {
class DiffuseEffect;
class Light;
class LightControllerProvider;
class SceneNode;
class SceneRenderNode;
typedef scoped_refptr<Light> LightPtr;
typedef scoped_refptr<SceneNode> SceneNodePtr;
typedef scoped_refptr<DiffuseEffect> DiffuseEffectPtr;

class LightController : public ::base::RefCounted<LightController> {
 public:
  explicit LightController(SceneRenderNode* node);
  azer::Mesh* GetLightMesh() { return light_mesh_.get();}
  virtual void Update(const azer::FrameArgs& args) = 0;
  virtual void Render(azer::Renderer* renderer) = 0;
 protected:
  azer::MeshPtr light_mesh_;
  DiffuseEffectPtr effect_;
  SceneRenderNode* node_;
  scoped_refptr<LightControllerProvider> provider_;
  DISALLOW_COPY_AND_ASSIGN(LightController);
};

class PointLightController : public LightController {
 public:
  explicit PointLightController(SceneRenderNode* node);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  void InitMesh();
  void InitControllerMesh();
  azer::MeshPtr controller_mesh_;
  DISALLOW_COPY_AND_ASSIGN(PointLightController);
};

class SpotLightController : public LightController {
 public:
  explicit SpotLightController(SceneRenderNode* node);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  void InitMesh();
  void InitControllerMesh();

  azer::MeshPtr controller_mesh_;
  static const float kTopRadius;
  DISALLOW_COPY_AND_ASSIGN(SpotLightController);
};

class DirLightController : public LightController {
 public:
  explicit DirLightController(SceneRenderNode* node);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  void InitMesh();
  void InitControllerMesh();
  DISALLOW_COPY_AND_ASSIGN(DirLightController);
};

class LightControllerProvider : public azer::EffectParamsProvider {
 public:
  LightControllerProvider(SceneRenderNode* node);
  void SetLocalTransform(const azer::Matrix4& local);

  // override from EffectParamsProvider
  void UpdateParams(const azer::FrameArgs& args) override;
  const azer::Vector4& color() const { return color_;}
  const azer::Vector4& emission() const { return emission_;}
  const azer::Matrix4& GetWorld() const { return world_;}
  const azer::Matrix4& GetPV() const;
 private:
  azer::Vector4 color_;
  azer::Vector4 emission_;
  azer::Matrix4 world_;
  azer::Matrix4 local_transform_;
  SceneRenderNode* node_;
  DISALLOW_COPY_AND_ASSIGN(LightControllerProvider);
};

class LightControllerEffectAdapter : public azer::EffectParamsAdapter {
 public:
  LightControllerEffectAdapter();
  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* e, const azer::EffectParamsProvider* params)
      const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LightControllerEffectAdapter);
};
}  // namespace lord
