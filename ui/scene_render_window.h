#pragma once

#include "base/memory/scoped_ptr.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_render_tree.h"
#include "lordaeron/scene/scene_renderer.h"
#include "lordaeron/ui/render_window.h"

namespace lord {
class SceneNode;
class RendererInfoPane;
class SceneRenderWindow : public RenderWindow {
 public:
  SceneRenderWindow(const gfx::Rect& init_bounds, nelf::RenderLoop* render_loop);
  ~SceneRenderWindow();

  void SetSceneNode(SceneNodePtr node);
  void SetSceneRenderer(scoped_ptr<SceneRenderer> scene_renderer);

  void OnInit() override;
  void OnUpdateFrame(const azer::FrameArgs& args) override;
  void OnRenderFrame(const azer::FrameArgs& args, azer::Renderer* renderer) override;
 protected:
  SceneNodePtr root_;
  scoped_ptr<SceneRenderer> scene_renderer_;
  DISALLOW_COPY_AND_ASSIGN(SceneRenderWindow);
};
}  // namespace lord
