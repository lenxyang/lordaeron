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
class RenderEnvNode;
typedef scoped_refptr<RenderEnvNode> RenderEnvNodePtr;

class RenderEnvNodeDelegate {
 public:
};

class RenderEnvNode : public azer::EffectParamsProvider {
 public:
  RenderEnvNode();
  explicit RenderEnvNode(RenderEnvNode* parent);
  ~RenderEnvNode();

  void AddLightNode(SceneNode* node);
  const Lights& lights() const { return all_lights_;}
  void UpdateParams(const azer::FrameArgs& args) override;
  
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
  SceneNodes light_nodes_;
  RenderEnvNode* parent_;
  Lights all_lights_;
  std::vector<RenderEnvNodePtr> children_;
  DISALLOW_COPY_AND_ASSIGN(RenderEnvNode);
};

}  // namespace lord
