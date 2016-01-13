#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node_traverse.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/render_env_node.h"
#include "lordaeron/scene/scene_render.h"

namespace lord {
class SceneNode;
class LightController;
class UISceneRender;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class LordEnvNodeDelegate : public RenderEnvNodeDelegate {
 public:
  explicit LordEnvNodeDelegate(RenderEnvNode* envnode);
  const Lights& lights() const { return all_lights_;}

  void Init(SceneNode* render_node, RenderNode* node) override;
  void OnUpdateNode(const azer::FrameArgs& args) override;
 private:
  SceneNodes light_nodes_;
  Lights all_lights_;
  DISALLOW_COPY_AND_ASSIGN(LordEnvNodeDelegate);
};

class LordSceneBVParamsAdapter : public azer::EffectParamsAdapter {
 public:
  static const azer::EffectAdapterKey kAdapterKey;
  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* effect,
             const azer::EffectParamsProvider* params) const override;
};

class LordSceneBVRenderProvider : public azer::EffectParamsProvider {
 public:
  explicit LordSceneBVRenderProvider(RenderNode* node);
  void Update();
  const azer::Vector4& color() const { return color_;}
  const azer::Matrix4& GetWorld() const { return world_;}
  const azer::Matrix4& GetPV() const;
 private:
  azer::Vector4 color_;
  azer::Matrix4 scale_;
  azer::Matrix4 world_;
  RenderNode* node_;
  DISALLOW_COPY_AND_ASSIGN(LordSceneBVRenderProvider);
};

class LordObjectNodeRenderDelegate : public RenderNodeDelegate {
 public:
  explicit LordObjectNodeRenderDelegate(RenderNode* node,
                                        UISceneRender* renderer);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  bool Init();
  azer::MeshPtr mesh_;
  azer::MeshPtr bounding_mesh_;
  azer::MeshPtr normal_mesh_;
  UISceneRender* tree_renderer_;
  scoped_refptr<LordSceneBVRenderProvider> bvprovider_;
  DISALLOW_COPY_AND_ASSIGN(LordObjectNodeRenderDelegate);
};

class LordLampNodeRenderDelegate : public RenderNodeDelegate {
 public:
  explicit LordLampNodeRenderDelegate(RenderNode* node);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  bool Init();
  LightController* controller_;
  DISALLOW_COPY_AND_ASSIGN(LordLampNodeRenderDelegate);
};

class UISceneRender : public SceneRender {
 public:
  UISceneRender();
  void AddBoundingVolumnMesh(azer::MeshPtr mesh) { bvmesh_.push_back(mesh);}
 protected:
  void OnFrameUpdateBegin(const azer::FrameArgs& args) override;
  void OnFrameRenderBegin(azer::Renderer* renderer) override;
  bool OnUpdateNode(RenderNode* node, const azer::FrameArgs& args) override;
  bool OnRenderNode(RenderNode* node, azer::Renderer* renderer) override;
  void OnFrameUpdateEnd(const azer::FrameArgs& args) override;
  void OnFrameRenderEnd(azer::Renderer* renderer) override;

  azer::DepthStencilStatePtr depth_state_;
  std::vector<RenderNode*> blending_node_;
  std::vector<azer::MeshPtr> bvmesh_;
  std::vector<azer::MeshPtr> normal_mesh_;
  DISALLOW_COPY_AND_ASSIGN(UISceneRender);
};

azer::MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node);
azer::MeshPtr CreateNormalLineMeshForSceneNode(SceneNode* node);
}  // namespace lord
