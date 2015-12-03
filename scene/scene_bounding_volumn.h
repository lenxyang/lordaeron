#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {

class SceneNode;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class SceneBVParamsAdapter : public azer::EffectParamsAdapter {
 public:
  static const azer::EffectAdapterKey kAdapterKey;
  azer::EffectAdapterKey key() const override;
  void Apply(azer::Effect* effect,
             const azer::EffectParamsProvider* params) const override;
};

class SceneBVRenderNode : public SceneRenderNode {
 public:
  explicit SceneBVRenderNode(SceneNode* node);
  ~SceneBVRenderNode() override;
  void Init() override;
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;

  const azer::Vector4& color() const { return color_;}
 private:
  azer::Vector4 color_;
  DISALLOW_COPY_AND_ASSIGN(SceneBVRenderNode);
};

azer::MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node);
}  // namespace lord
