#pragma once

#include "base/memory/scoped_ptr.h"
#include "azer/render/render.h"
#include "lordaeron/scene/scene_node_traverse.h"

namespace lord {
class SceneContext;
class SceneNode;
class SceneEnvironment;

class SceneArgsUpdator : public SceneNodeTraverseDelegate {
 public:
  SceneArgsUpdator(SceneEnvironment* env);
  ~SceneArgsUpdator();

  void SetFrameArgs(const azer::FrameArgs* args) { args_ = args;}
  void OnTraverseBegin(SceneNode* root) override;
  bool OnTraverseNodeEnter(SceneNode* node) override;
  void OnTraverseNodeExit(SceneNode* node) override;
  void OnTraverseEnd() override;
  const std::vector<azer::MeshPtr>& mesh() const { return mesh_;}
 private:
  void UpdateNodeWorld(SceneNode* node);
  SceneEnvironment* environment_;
  std::vector<azer::MeshPtr> mesh_;
  const azer::FrameArgs* args_;
  DISALLOW_COPY_AND_ASSIGN(SceneArgsUpdator);
};

class SceneRender {
 public:
  SceneRender(SceneContext* context, SceneNode* root);
  ~SceneRender();

  void Update(const azer::FrameArgs& args);
  void Draw(azer::Renderer* renderer, azer::Effect* effect,
            azer::PrimitiveTopology primitive);
 private:
  SceneContext* context_;
  SceneNode* root_;
  scoped_ptr<SceneArgsUpdator> delegate_;
  DISALLOW_COPY_AND_ASSIGN(SceneRender);
};
}  // namespace lord
