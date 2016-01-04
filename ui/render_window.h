#pragma once

#include "base/memory/scoped_ptr.h"
#include "nelf/nelf.h"
#include "lordaeron/env.h"
#include "lordaeron/scene/scene_node.h"

namespace azer {
class Camera;
class CoordinateGrid;
class Renderer;
}

namespace lord {
class RendererInfoPane;

class RenderWindow : public nelf::MainFrame,
                          public nelf::RenderDelegate,
                          public views::WidgetObserver {
 public:
  RenderWindow(const gfx::Rect& init_bounds);
  RenderWindow(const gfx::Rect& init_bounds,
                    nelf::RenderLoop* render_loop);
  ~RenderWindow();

  azer::Camera* mutable_camera() { return &camera_;}
  const azer::Camera& camera() const { return camera_;}
  nelf::RenderLoop* GetRenderLoop() { return render_loop_.get();}
 protected:
  virtual void OnInit() {}
  virtual void OnUpdateFrame(const azer::FrameArgs& args) = 0;
  virtual void OnRenderFrame(const azer::FrameArgs& args, 
                             azer::Renderer* renderer) = 0;
 protected:
  // override from nelf::RenderDelegate
  bool Initialize() override;
  void OnUpdate(const azer::FrameArgs& args) override;
  void OnRender(const azer::FrameArgs& args) override;

  // override from views::WidgetObserver
  void OnWidgetBoundsChanged(views::Widget* widget, 
                             const gfx::Rect& new_bounds) override;
  void OnWidgetDestroying(views::Widget* widget) override;
 private:
  void InitRenderView();
  virtual void OnAfterWidgetInit() override;
  nelf::RenderView* render_view_;
  azer::RenderStatePtr render_state_;
  scoped_refptr<nelf::RenderLoop> render_loop_;
  azer::Camera camera_;
  RendererInfoPane* renderer_pane_;
  DISALLOW_COPY_AND_ASSIGN(RenderWindow);
};
}  // namespace lord
