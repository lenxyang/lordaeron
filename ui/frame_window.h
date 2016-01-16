#pragma once

#include "base/memory/scoped_ptr.h"
#include "nelf/nelf.h"
#include "lordaeron/env.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/render_window.h"

namespace azer {
class Camera;
class Renderer;
}

namespace lord {
class CameraOverlay;
class InteractiveContext;
class RendererInfoPane;
class CoordinateGrid;

class FrameWindow : public RenderWindow {
 public:
  explicit FrameWindow(const gfx::Rect& init_bounds);
  FrameWindow(const gfx::Rect& init_bounds, nelf::RenderLoop* render_loop);
  ~FrameWindow();
  InteractiveContext* GetInteractive() { return  interactive_.get();}
  void set_draw_gridline(bool draw) { draw_gridline_ = draw;}
  bool draw_gridline() const { return draw_gridline_;}
  SceneNode* root() { return root_;}

  virtual SceneNodePtr InitScene() = 0;

  // override from nelf::RenderDelegate
  bool Initialize() override;
  void OnUpdate(const azer::FrameArgs& args) override;
  void OnRender(const azer::FrameArgs& args) override;
 private:
  void OnInitUI();
  scoped_ptr<CameraOverlay> camera_overlay_;
  scoped_ptr<CoordinateGrid> gridline_;
  bool draw_gridline_;
  scoped_ptr<InteractiveContext> interactive_;
  SceneNodePtr root_;
  DISALLOW_COPY_AND_ASSIGN(FrameWindow);
};
}  // namespace lord
