#pragma once

#include <vector>
#include "azer/render/render.h"

namespace lord {
class DiffuseEffect;
class Light;
class LightControllerProvider;
class SceneNode;
class RenderNode;
typedef scoped_refptr<Light> LightPtr;
typedef scoped_refptr<SceneNode> SceneNodePtr;
typedef scoped_refptr<DiffuseEffect> DiffuseEffectPtr;

class LightController : public ::base::RefCounted<LightController> {
 public:
  explicit LightController(RenderNode* node);
  azer::Mesh* GetLightMesh() { return light_mesh_.get();}
  virtual void Update(const azer::FrameArgs& args) = 0;
  virtual void Render(azer::Renderer* renderer) = 0;
 protected:
  azer::MeshPtr light_mesh_;
  DiffuseEffectPtr effect_;
  RenderNode* node_;
  scoped_refptr<LightControllerProvider> provider_;
  DISALLOW_COPY_AND_ASSIGN(LightController);
};

class PointLightController : public LightController {
 public:
  explicit PointLightController(RenderNode* node);
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
  explicit SpotLightController(RenderNode* node);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  void InitMesh();
  void InitControllerMesh();
  void CreateCrossCircle(float mid, Light* light);
  void CreateBorderLine(Light* light);

  azer::RenderStatePtr render_state_;
  azer::MeshPtr controller_mesh_;
  azer::MeshPtr line_mesh_;
  static const float kTopRadius;
  DISALLOW_COPY_AND_ASSIGN(SpotLightController);
};

class DirLightController : public LightController {
 public:
  explicit DirLightController(RenderNode* node);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  void InitMesh();
  void InitControllerMesh();
  DISALLOW_COPY_AND_ASSIGN(DirLightController);
};

class LightControllerProvider : public azer::EffectParamsProvider {
 public:
  LightControllerProvider(RenderNode* node);
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
  RenderNode* node_;
  DISALLOW_COPY_AND_ASSIGN(LightControllerProvider);
};

class LightControllerColorProvider : public azer::EffectParamsProvider {
 public:
  explicit LightControllerColorProvider(const azer::Vector4& color);
  LightControllerColorProvider();
  // override from EffectParamsProvider
  void UpdateParams(const azer::FrameArgs& args) override {}
  const azer::Vector4& color() const { return color_;}
 private:
  azer::Vector4 color_;
  DISALLOW_COPY_AND_ASSIGN(LightControllerColorProvider);
};

class LightControllerEffectAdapter : public azer::EffectParamsAdapter {
 public:
  static const azer::EffectAdapterKey kAdapterKey;
  LightControllerEffectAdapter();
  azer::EffectAdapterKey key() const override { return kAdapterKey;}
  void Apply(azer::Effect* e, const azer::EffectParamsProvider* params)
      const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LightControllerEffectAdapter);
};

class LightControllerColorEffectAdapter : public azer::EffectParamsAdapter {
 public:
  static const azer::EffectAdapterKey kAdapterKey;
  LightControllerColorEffectAdapter();
  azer::EffectAdapterKey key() const override { return kAdapterKey;}
  void Apply(azer::Effect* e, const azer::EffectParamsProvider* params)
      const override;
 private:
  DISALLOW_COPY_AND_ASSIGN(LightControllerColorEffectAdapter);
};
}  // namespace lord
