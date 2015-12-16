#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node_traverse.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {
class SceneNode;
class LightController;
class SimpleRenderTreeRenderer;

class LoadSceneBVParamsAdapter : public azer::EffectParamsAdapter {
 public:
  static const azer::EffectAdapterKey kAdapterKey;
  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* effect,
             const azer::EffectParamsProvider* params) const override;
};

class LoadSceneBVRenderProvider : public azer::EffectParamsProvider {
 public:
  explicit LoadSceneBVRenderProvider(SceneRenderNode* node);
  void UpdateParams(const azer::FrameArgs& args) override;
  const azer::Vector4& color() const { return color_;}
  const azer::Matrix4& GetWorld() const { return world_;}
  const azer::Matrix4& GetPV() const;
 private:
  azer::Vector4 color_;
  azer::Matrix4 scale_;
  azer::Matrix4 world_;
  SceneRenderNode* node_;
  DISALLOW_COPY_AND_ASSIGN(LoadSceneBVRenderProvider);
};

class LordObjectNodeRenderDelegate : public SceneRenderNodeDelegate {
 public:
  explicit LordObjectNodeRenderDelegate(SceneRenderNode* node,
                                        SimpleRenderTreeRenderer* renderer);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  bool Init();
  azer::MeshPtr mesh_;
  azer::MeshPtr bounding_mesh_;
  azer::MeshPtr normal_mesh_;
  SimpleRenderTreeRenderer* tree_renderer_;
  DISALLOW_COPY_AND_ASSIGN(LordObjectNodeRenderDelegate);
};

class LordLampNodeRenderDelegate : public SceneRenderNodeDelegate {
 public:
  explicit LordLampNodeRenderDelegate(SceneRenderNode* node);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  bool Init();
  LightController* controller_;
  DISALLOW_COPY_AND_ASSIGN(LordLampNodeRenderDelegate);
};

class LoadSceneRenderNodeDelegateFactory : public SceneRenderNodeDelegateFactory {
 public:
  LoadSceneRenderNodeDelegateFactory(SimpleRenderTreeRenderer* renderer)
      : tree_renderer_(renderer) {}
  scoped_ptr<SceneRenderNodeDelegate> CreateDelegate(SceneRenderNode* node) override;
 private:
  SimpleRenderTreeRenderer* tree_renderer_;
};

class SimpleRenderTreeRenderer {
 public:
  SimpleRenderTreeRenderer();
  void AddBoundingVolumnMesh(azer::MeshPtr mesh) { bvmesh_.push_back(mesh);}
  void SetSceneNode(SceneRenderNode* root);
  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);
 private:
  void UpdateNode(SceneRenderNode* node, const azer::FrameArgs& args);
  void RenderNode(SceneRenderNode* node, azer::Renderer* renderer);
  SceneRenderNode* root_;
  std::vector<SceneRenderNode*> blending_node_;
  std::vector<azer::MeshPtr> bvmesh_;
  std::vector<azer::MeshPtr> normal_mesh_;
  DISALLOW_COPY_AND_ASSIGN(SimpleRenderTreeRenderer);
};

azer::MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node);
azer::MeshPtr CreateNormalLineMeshForSceneNode(SceneNode* node);
}  // namespace lord
