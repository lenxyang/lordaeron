#pragma once

#include "base/memory/scoped_ptr.h"
#include "nelf/nelf.h"
#include "lordaeron/context.h"

namespace azer {
class Camera;
class CoordinateGrid;
class Renderer;
}

namespace lord {
class CameraOverlay;
class RendererInfoPane;

class SimpleRenderWindow : public nelf::Mainframe,
                          public nelf::RenderDelegate,
                          public views::WidgetObserver {
 public:
  SimpleRenderWindow(const gfx::Rect& init_bounds);
  SimpleRenderWindow(const gfx::Rect& init_bounds,
                    nelf::RenderLoop* render_loop);
  ~SimpleRenderWindow();

  nelf::RenderLoop* GetRenderLoop() { return render_loop_.get();}

  virtual void OnInitScene() = 0;
  virtual void OnInitUI() = 0;
  virtual void OnUpdateFrame(const azer::FrameArgs& args) = 0;
  virtual void OnRenderFrame(const azer::FrameArgs& args, 
                             azer::Renderer* renderer) = 0;

  // override from nelf::RenderDelegate
  bool Initialize() override;
  void OnUpdate(const azer::FrameArgs& args) override;
  void OnRender(const azer::FrameArgs& args) override;

  // override from views::WidgetObserver
  void OnWidgetBoundsChanged(views::Widget* widget, 
                             const gfx::Rect& new_bounds) override;
  void OnWidgetDestroying(views::Widget* widget) override;

  azer::Camera* mutable_camera() { return &camera_;}
  const azer::Camera& camera() const { return camera_;}
 private:
  void InitRenderView();
  virtual void OnAfterWidgetInit() override;
  nelf::RenderView* render_view_;
  scoped_refptr<nelf::RenderLoop> render_loop_;

  azer::Camera camera_;
  RendererInfoPane* renderer_pane_;
  scoped_ptr<CameraOverlay> camera_overlay_;
  scoped_ptr<azer::CoordinateGrid> gridline_;
  DISALLOW_COPY_AND_ASSIGN(SimpleRenderWindow);
};
}  // namespace lord
