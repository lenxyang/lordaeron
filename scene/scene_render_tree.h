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

class SceneRenderEnvNode : public azer::EffectParamsProvider {
 public:
  SceneRenderEnvNode();
  explicit SceneRenderEnvNode(SceneRenderEnvNode* parent);
  ~SceneRenderEnvNode();

  void AddLightNode(SceneNode* node);
  const Lights& lights() const { return all_lights_;}
  void UpdateParams(const azer::FrameArgs& args) override;
  
  SceneRenderEnvNode* root();
  const SceneRenderEnvNode* root() const {
    return const_cast<const SceneRenderEnvNode*>(
        const_cast<SceneRenderEnvNode*>(this)->root());
  }
  SceneRenderEnvNode* parent();
  const SceneRenderEnvNode* parent() const {
    return const_cast<const SceneRenderEnvNode*>(
        const_cast<SceneRenderEnvNode*>(this)->parent());
  }
  int32 child_count() const;
  SceneRenderEnvNode* child_at(int32 index);
  void AddChild(SceneRenderEnvNode* child);
  bool RemoveChild(SceneRenderEnvNode* child);
  bool Contains(SceneRenderEnvNode* child) const;
  int32 GetIndexOf(SceneRenderEnvNode* child) const;
  std::string DumpTree() const;
  std::string DumpNode(const SceneRenderEnvNode* node, int32 dump) const;
 private:
  SceneNodes light_nodes_;
  SceneRenderEnvNode* parent_;
  Lights all_lights_;
  std::vector<SceneRenderEnvNodePtr> children_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderEnvNode);
};

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

class SceneRenderNodeDelegateFactory {
 public:
  virtual scoped_ptr<SceneRenderNodeDelegate> CreateDelegate(SceneRenderNode* n) = 0;
};

class SceneRenderTreeBuilder : public SceneNodeTraverseDelegate {
 public:
  explicit SceneRenderTreeBuilder(SceneRenderNodeDelegateFactory* factory);
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
  SceneRenderNodeDelegateFactory* factory_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderTreeBuilder);
};
}  // namespace lord
