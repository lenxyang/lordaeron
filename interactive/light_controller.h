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

// why not put Use LightControler as EffectParamsProvider
// because
class LightControllerProvider : public azer::EffectParamsProvider {
 public:
  explicit LightControllerProvider(RenderNode* node);
  void Update();
  const azer::Vector4& color() const { return color_;}
  const azer::Vector4& emission() const { return emission_;}
  const azer::Matrix4& GetWorld() const { return world_;}
  const azer::Matrix4& GetPV() const;
 private:
  azer::Vector4 color_;
  azer::Vector4 emission_;
  azer::Matrix4 world_;
  RenderNode* node_;
  DISALLOW_COPY_AND_ASSIGN(LightControllerProvider);
};
class LightController : public base::RefCounted<LightController> {
 public:
  explicit LightController(RenderNode* node);
  azer::Mesh* GetLightMesh() { return light_mesh_.get();}
  virtual void Update(const azer::FrameArgs& args);
  virtual void Render(azer::Renderer* renderer);
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
  void CreateFrameLine(float mid, Light* light);
  void CreateBorderLine(Light* light);

  azer::RasterizerStatePtr rasterizer_state_;
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

}  // namespace lord
