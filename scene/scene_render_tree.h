#pragma once

#include <deque>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base/memory/ref_counted.h"
#include "azer/render/render.h"
#include "lordaeron/effect/light.h"
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
  const Lights& lights() const { return lights_;}
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
  Lights lights_;
  SceneRenderEnvNode* parent_;
  Lights all_lights_;
  std::vector<SceneRenderEnvNodePtr> children_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderEnvNode);
};

class SceneRenderNode : public azer::EffectParamsProvider {
 public:
  SceneRenderNode(SceneNode* node, SceneRenderEnvNode* envnode);
  SceneRenderNode(SceneNode* node, SceneRenderEnvNode* envnode,
                  const azer::Camera* camera);
  virtual ~SceneRenderNode();

  SceneNode* GetSceneNode() { return node_;}
  const SceneNode* GetSceneNode() const { return node_;}
  SceneRenderEnvNode* GetEnvNode() { return envnode_;}
  const SceneRenderEnvNode* GetEnvNode() const { return envnode_;}
  void  SetEnvNode(SceneRenderEnvNode* node);
  void UpdateParams(const azer::FrameArgs& args) override;
  const azer::Matrix4& GetWorld() const { return world_;}
  const azer::Matrix4& GetPVW() const { return pvw_;}
  const azer::Camera* camera() const;

  // virtual function;
  virtual void Init();
  virtual void Update(const azer::FrameArgs& args);
  virtual void Render(azer::Renderer* renderer);

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
 private:
  void AddMesh(azer::Mesh* mesh);
  SceneRenderNode* parent_;
  std::vector<SceneRenderNodePtr> children_;

  SceneNode* node_;
  SceneRenderEnvNodePtr envnode_;
  const azer::Camera* camera_;
  azer::MeshPtr mesh_;
  azer::Matrix4 world_;
  azer::Matrix4 pvw_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderNode);
};

class SceneRenderTreeBuilder : public SceneNodeTraverseDelegate {
 public:
  SceneRenderTreeBuilder();
  ~SceneRenderTreeBuilder();

  void Build(SceneNode* node, const azer::Camera* camera);
  SceneRenderNodePtr GetRenderNodeRoot();

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
