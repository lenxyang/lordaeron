#pragma once

#include <deque>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "azer/render/render.h"
#include "lordaeron/render/light.h"
#include "lordaeron/scene/scene_node_traverse.h"

namespace lord {
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
};

class SceneRenderNode : public ::baee::RefCounted<SceneRenderNode> {
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

  virtual void Update(const azer::FrameArgs& args);
  virtual void Render(Renderer* renderer);
 private:
  void AddMesh(azer::Mesh* mesh);
  SceneRenderNode* parent_;
  SceneNode* node_;
  SceneRenderEnvNodePtr envnode_;
  std::vector<azer::MeshPtr> mesh_;
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
}  // namespace lord
