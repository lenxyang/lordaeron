#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node_traverse.h"
#include "lordaeron/scene/scene_render_tree.h"
#include "lordaeron/scene/scene_renderer.h"

namespace lord {
class SceneNode;
class LightController;
class UISceneRenderer;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class LordSceneBVParamsAdapter : public azer::EffectParamsAdapter {
 public:
  static const azer::EffectAdapterKey kAdapterKey;
  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* effect,
             const azer::EffectParamsProvider* params) const override;
};

class LordSceneBVRenderProvider : public azer::EffectParamsProvider {
 public:
  explicit LordSceneBVRenderProvider(SceneRenderNode* node);
  void UpdateParams(const azer::FrameArgs& args) override;
  const azer::Vector4& color() const { return color_;}
  const azer::Matrix4& GetWorld() const { return world_;}
  const azer::Matrix4& GetPV() const;
 private:
  azer::Vector4 color_;
  azer::Matrix4 scale_;
  azer::Matrix4 world_;
  SceneRenderNode* node_;
  DISALLOW_COPY_AND_ASSIGN(LordSceneBVRenderProvider);
};

class LordObjectNodeRenderDelegate : public SceneRenderNodeDelegate {
 public:
  explicit LordObjectNodeRenderDelegate(SceneRenderNode* node,
                                        UISceneRenderer* renderer);
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
 private:
  bool Init();
  azer::MeshPtr mesh_;
  azer::MeshPtr bounding_mesh_;
  azer::MeshPtr normal_mesh_;
  UISceneRenderer* tree_renderer_;
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

class UISceneRenderer : public SceneRenderer {
 public:
  UISceneRenderer();
  void AddBoundingVolumnMesh(azer::MeshPtr mesh) { bvmesh_.push_back(mesh);}
 protected:
  void OnFrameUpdateBegin(const azer::FrameArgs& args) override;
  void OnFrameRenderBegin(azer::Renderer* renderer) override;
  bool UpdateNode(SceneRenderNode* node, const azer::FrameArgs& args) override;
  bool RenderNode(SceneRenderNode* node, azer::Renderer* renderer) override;
  void OnFrameUpdateEnd(const azer::FrameArgs& args) override;
  void OnFrameRenderEnd(azer::Renderer* renderer) override;

  std::vector<SceneRenderNode*> blending_node_;
  std::vector<azer::MeshPtr> bvmesh_;
  std::vector<azer::MeshPtr> normal_mesh_;
  DISALLOW_COPY_AND_ASSIGN(UISceneRenderer);
};

azer::MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node);
azer::MeshPtr CreateNormalLineMeshForSceneNode(SceneNode* node);
}  // namespace lord
