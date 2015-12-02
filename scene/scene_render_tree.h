#pragma once

#include <deque>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base/memory/ref_counted.h"
#include "azer/render/render.h"
#include "lordaeron/render/light.h"
#include "lordaeron/scene/scene_node_traverse.h"

namespace lord {
class SceneRenderEnvNode;
class SceneRenderNode;
typedef scoped_refptr<SceneRenderEnvNode> SceneRenderEnvNodePtr;
typedef scoped_refptr<SceneRenderNode> SceneRenderNodePtr;

class SceneRenderEnvNode : public azer::EffectParamsProvider {
 public:
  SceneRenderEnvNode();
  explicit SceneRenderEnvNode(SceneRenderEnvNode* parent);

  void AddLight(Light* light) { lights_.push_back(light);}
  void UpdateParams(const azer::FrameArgs& args) override;
  
  SceneRenderEnvNode* root();
  SceneRenderEnvNode* parent();
  int32 child_count() const;
  SceneRenderEnvNode* child_at(int32 index);
  void AddChild(SceneRenderEnvNode* child);
  bool Contains(SceneRenderEnvNode* child) const;
  int32 GetIndexOf(SceneRenderEnvNode* child) const;
 private:
  Lights lights_;
  SceneRenderEnvNode* parent_;
  Lights all_lights_;
  std::vector<SceneRenderEnvNodePtr> children_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderEnvNode);
};

class SceneRenderNode : public ::base::RefCounted<SceneRenderNode> {
 public:
  SceneRenderNode(SceneNode* node);
  virtual ~SceneRenderNode();

  SceneNode* GetSceneNode() { return node_;}
  SceneRenderEnvNode* GetRenderEnvNode() { return envnode_.get();}
  void SetSceneRenderEnvNode(SceneRenderEnvNode* n) { envnode_ = n;}

  SceneRenderNode* root();
  SceneRenderNode* parent();
  int32 child_count() const;
  SceneRenderNode* child_at(int32 index);
  void AddChild(SceneRenderNode* child);
  bool Contains(SceneRenderNode* child) const;
  int32 GetIndexOf(SceneRenderNode* child) const;
  const std::vector<SceneRenderNodePtr>& children() const { return children_;}

  virtual void Update(const azer::FrameArgs& args);
  virtual void Render(azer::Renderer* renderer);
 private:
  void AddMesh(azer::Mesh* mesh);
  SceneRenderNode* parent_;
  SceneNode* node_;
  SceneRenderEnvNodePtr envnode_;
  std::vector<SceneRenderNodePtr> children_;
  azer::MeshPtr mesh_;
  azer::Matrix4 world_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderNode);
};

class SceneRenderTreeBuilder : public SceneNodeTraverseDelegate {
 public:
  SceneRenderTreeBuilder();
  ~SceneRenderTreeBuilder();

  void Bulid(SceneNode* node);
  SceneRenderNodePtr GetRenderNodeRoot();
  SceneRenderEnvNodePtr GetRenderEnvNodeRoot();

  // override from SceneNodeTraverseDelegate
  void OnTraverseBegin(SceneNode* root) override;
  bool OnTraverseNodeEnter(SceneNode* node) override;
  void OnTraverseNodeExit(SceneNode* node) override;
  void OnTraverseEnd() override;
 private:
  void UpdateNodeWorld(SceneNode* node);
  SceneRenderNode* cur_;
  SceneRenderNodePtr root_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderTreeBuilder);
};

class SimpleRenderTreeRenderer {
 public:
  explicit SimpleRenderTreeRenderer(SceneRenderNode* root);
  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);
 private:
  void UpdateNode(SceneRenderNode* node, const azer::FrameArgs& args);
  void RenderNode(SceneRenderNode* node, azer::Renderer* renderer);
  SceneRenderNode* root_;
  DISALLOW_COPY_AND_ASSIGN(SimpleRenderTreeRenderer);
};
}  // namespace lord
