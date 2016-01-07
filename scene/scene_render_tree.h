#pragma once

#include <deque>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base/memory/ref_counted.h"
#include "azer/render/render.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/scene/scene_node_traverse.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
class SceneRenderEnvNode;
class SceneRenderNode;
typedef scoped_refptr<SceneRenderEnvNode> SceneRenderEnvNodePtr;
typedef scoped_refptr<SceneRenderNode> SceneRenderNodePtr;

class SceneRenderNodeDelegate {
 public:
  explicit SceneRenderNodeDelegate(SceneRenderNode* node);
  virtual void Update(const azer::FrameArgs& args) = 0;
  virtual void Render(azer::Renderer* renderer) = 0;
  SceneNode* GetSceneNode();
  const SceneNode* GetSceneNode() const;
  SceneRenderNode* GetSceneRenderNode();
  const SceneRenderNode* GetSceneRenderNode() const;
 protected:
  SceneRenderNode* node_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderNodeDelegate);
};

class SceneRenderNode : public azer::EffectParamsProvider {
 public:
  explicit SceneRenderNode(SceneNode* node);
  virtual ~SceneRenderNode();
  void SetDelegate(scoped_ptr<SceneRenderNodeDelegate> delegate);

  SceneNode* GetSceneNode() { return node_;}
  const SceneNode* GetSceneNode() const { return node_;}
  SceneRenderEnvNode* GetEnvNode() { return envnode_;}
  const SceneRenderEnvNode* GetEnvNode() const { return envnode_;}
  void  SetEnvNode(SceneRenderEnvNode* node);

  void UpdateParams(const azer::FrameArgs& args) override;
  const azer::Matrix4& GetWorld() const { return world_;}
  const azer::Matrix4& GetPVW() const { return pvw_;}
  const azer::Matrix4& GetPV() const { return pv_;}
  const azer::Camera* camera() const;
  void SetCamera(const azer::Camera* camera);

  // virtual function;
  bool Init();
  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);

  SceneRenderNode* root();
  const SceneRenderNode* root() const {
    return const_cast<const SceneRenderNode*>(
        const_cast<SceneRenderNode*>(this)->root());
  }
  SceneRenderNode* parent();
  const SceneRenderNode* parent() const {
    return const_cast<const SceneRenderNode*>(
        const_cast<SceneRenderNode*>(this)->parent());
  }
  int32 child_count() const;
  SceneRenderNode* child_at(int32 index);
  void AddChild(SceneRenderNode* child);
  bool RemoveChild(SceneRenderNode* child);
  bool Contains(SceneRenderNode* child) const;
  int32 GetIndexOf(SceneRenderNode* child) const;
  const std::vector<SceneRenderNodePtr>& children() const { return children_;}
  std::string DumpTree() const;
  std::string DumpNode(const SceneRenderNode* node, int32 depth) const;
 protected:
  void CalcParams(const azer::FrameArgs& args);
  SceneRenderNode* parent_;
  std::vector<SceneRenderNodePtr> children_;

  SceneNode* node_;
  scoped_ptr<SceneRenderNodeDelegate> delegate_;
  SceneRenderEnvNodePtr envnode_;
  const azer::Camera* camera_;
  azer::Matrix4 world_;
  azer::Matrix4 pvw_;
  azer::Matrix4 pv_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderNode);
};

class SceneNodeRenderDelegateFactory {
 public:
  virtual scoped_ptr<SceneRenderNodeDelegate> CreateDelegate(SceneRenderNode* n) = 0;
};

class SceneRenderTreeBuilder : public SceneNodeTraverseDelegate {
 public:
  explicit SceneRenderTreeBuilder(SceneNodeRenderDelegateFactory* factory);
  ~SceneRenderTreeBuilder();

  SceneRenderNodePtr Build(SceneNode* node, const azer::Camera* camera);

  // override from SceneNodeTraverseDelegate
  void OnTraverseBegin(SceneNode* root) override;
  bool OnTraverseNodeEnter(SceneNode* node) override;
  void OnTraverseNodeExit(SceneNode* node) override;
  void OnTraverseEnd() override;
 private:
  void UpdateNodeWorld(SceneNode* node);
  SceneRenderNode* cur_;
  SceneNodeRenderDelegateFactory* factory_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderTreeBuilder);
};
}  // namespace lord
