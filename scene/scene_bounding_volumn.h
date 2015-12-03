#pragma once

#include "azer/render/render.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {

/**
 * Scene Node 的 Bounding Volumn 有两种方法渲染
 * 简单的： 在 SceneRenderNode 当中增加一个 bounding_mesh_
 * 复杂的： 增加一棵单独的渲染树进行渲染
 * 这里使用复杂的方法渲染 Bounding volumn, 同时也作为例子使用
 * 需要注意一点： 简单的方法还有一个有点可以节省计算量
 * 
 */
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
  azer::MeshPtr bounding_mesh_;
  DISALLOW_COPY_AND_ASSIGN(SceneBVRenderNode);
};

class SceneBVRenderNodeCreator : public SceneRenderNodeCreator {
 public:
  SceneRenderNode* Create(SceneNode* node) override;
};

azer::MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node);
}  // namespace lord
