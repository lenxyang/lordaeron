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
class RenderNode;
class RenderEnvNode;
class RenderEnvNodeDelegate;
typedef scoped_refptr<RenderEnvNode> RenderEnvNodePtr;
typedef scoped_refptr<RenderEnvNodeDelegate> RenderEnvNodeDelegatePtr;

class RenderEnvNodeDelegate : public azer::EffectParamsProvider {
 public:
  explicit RenderEnvNodeDelegate(RenderEnvNode* envnode);

  RenderEnvNode* node() { return envnode_;}
  const RenderEnvNode* node() const { return envnode_;}

  virtual void Reset() = 0;
  virtual void VisitSceneNode(SceneNode* node, RenderNode* render_node) = 0;
  virtual void OnUpdateNode(const azer::FrameArgs& args) = 0;

  // override form azer::EffectParamsProvider
  // should not update there, because the object will shared by many
  // SceneNode, so it neednot update for every one,
  // we will update the params from OnUpdateNode
  void UpdateParams(const azer::FrameArgs& args) override;
 private:
  RenderEnvNode* envnode_;
  DISALLOW_COPY_AND_ASSIGN(RenderEnvNodeDelegate);
};

class RenderEnvNode : public ::base::RefCounted<RenderEnvNode> {
 public:
  RenderEnvNode();
  explicit RenderEnvNode(RenderEnvNode* parent);
  ~RenderEnvNode();

  void set_delegate(RenderEnvNodeDelegate* delegate);
  RenderEnvNodeDelegate* delegate() { return delegate_.get();}
  const RenderEnvNodeDelegate* delegate() const { return delegate_.get();}
  
  RenderEnvNode* root();
  const RenderEnvNode* root() const {
    return const_cast<const RenderEnvNode*>(
        const_cast<RenderEnvNode*>(this)->root());
  }
  RenderEnvNode* parent();
  const RenderEnvNode* parent() const {
    return const_cast<const RenderEnvNode*>(
        const_cast<RenderEnvNode*>(this)->parent());
  }
  int32 child_count() const;
  RenderEnvNode* child_at(int32 index);
  void AddChild(RenderEnvNode* child);
  bool RemoveChild(RenderEnvNode* child);
  bool Contains(RenderEnvNode* child) const;
  int32 GetIndexOf(RenderEnvNode* child) const;
  std::string DumpTree() const;
  std::string DumpNode(const RenderEnvNode* node, int32 dump) const;
 private:
  RenderEnvNode* parent_;
  std::vector<RenderEnvNodePtr> children_;
  RenderEnvNodeDelegatePtr delegate_;
  DISALLOW_COPY_AND_ASSIGN(RenderEnvNode);
};

}  // namespace lord
