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
typedef scoped_refptr<SceneRenderEnvNode> SceneRenderEnvNodePtr;

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

}  // namespace lord
