#include "lordaeron/ui/scene_render_window.h"

#include "ui/views/layout/layout_manager.h"
#include "azer/render/render.h"
#include "lordaeron/env.h"
#include "lordaeron/interactive/fps_camera_controller.h"
#include "lordaeron/scene/scene_render.h"
#include "lordaeron/ui/render_window.h"
#include "lordaeron/ui/renderer_info_pane.h"

namespace lord {
using azer::FrameArgs;;
using azer::Renderer;;
namespace {
const azer::Vector4 kRenderBgColor = azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
}
SceneRenderWindow::SceneRenderWindow(const gfx::Rect& init_bounds, 
                                     nelf::RenderLoop* render_loop)
    : RenderWindow(init_bounds, render_loop) {
}

SceneRenderWindow::~SceneRenderWindow() {
}

void SceneRenderWindow::SetSceneNode(SceneNodePtr node) {
  DCHECK(!root_.get());
  root_ = node;
}

void SceneRenderWindow::SetSceneRender(scoped_ptr<SceneRender> scene_renderer) {
  DCHECK(!scene_renderer_.get());
  scene_renderer_ = scene_renderer.Pass();
}

void SceneRenderWindow::OnInit() {
  DCHECK(root_.get());
  DCHECK(scene_renderer_.get());
  scene_renderer_->Init(root_, &camera());
}

void SceneRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  scene_renderer_->Update(args);
}

void SceneRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  scene_renderer_->Render(renderer);
}
}  // namespace lord
