#include "lordaeron/ui/window/render_window.h"

#include "ui/views/layout/layout_manager.h"

#include "azer/render/render.h"
#include "lordaeron/env.h"
#include "lordaeron/interactive/fps_camera_controller.h"
#include "lordaeron/ui/renderer_info_pane.h"

namespace lord {
using namespace azer;
namespace {
const Vector4 kRenderBgColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
}
RenderWindow::RenderWindow(const gfx::Rect& init_bounds)
    : nelf::MainFrame(init_bounds, LordEnv::instance()->GetNelfContext()),
      render_view_(NULL) {
  Vector3 camera_pos(0.0f, 1.0f, 5.0f);
  Vector3 lookat(0.0f, 1.0f, 0.0f);
  Vector3 up(0.0f, 1.0f, 0.0f);
  camera_.reset(camera_pos, lookat, up);
  clear_color_ = kRenderBgColor;
}

RenderWindow::RenderWindow(const gfx::Rect& init_bounds, 
                           nelf::RenderLoop* render_loop)
    : nelf::MainFrame(init_bounds, LordEnv::instance()->GetNelfContext()),
      render_view_(NULL),
      render_loop_(render_loop) {
  Vector3 camera_pos(0.0f, 1.0f, 5.0f);
  Vector3 lookat(0.0f, 1.0f, 0.0f);
  Vector3 up(0.0f, 1.0f, 0.0f);
  camera_.reset(camera_pos, lookat, up);
  clear_color_ = kRenderBgColor;
}

RenderWindow::~RenderWindow() {
}

void RenderWindow::InitRenderView() {
  if (!render_loop_.get()) {
    render_loop_ = new nelf::RenderLoop(this);
  }

  render_view_ = new nelf::RenderView(render_loop_.get(), this);
  SetLayoutManager(new views::FillLayout);
  AddChildView(render_view_);
  render_loop_->AddRenderView(render_view_);
}

void RenderWindow::OnAfterWidgetInit() {
  nelf::MainFrame::OnAfterWidgetInit();
  InitRenderView();
}

// override from nelf::RenderDelegate
bool RenderWindow::Initialize() {
  renderer_pane_ = new RendererInfoPane;
  renderer_pane_->SetBounds(10, 10, 180, 120);
  this->window()->AddChildView(renderer_pane_);
  this->window()->SetRenderUI(true);
  window()->GetWidget()->AddObserver(this);
  OnInit();
  return true;
}

void RenderWindow::OnUpdate(const FrameArgs& args) {
  Renderer* renderer = window()->GetRenderer().get();
  OnUpdateFrame(args);
  renderer_pane_->Update(renderer, args);
}

void RenderWindow::OnRender(const FrameArgs& args) {
  Renderer* renderer = window()->GetRenderer().get();
  renderer->Use();
  renderer->Clear(clear_color_);
  renderer->ClearDepthAndStencil();
  OnRenderFrame(args, renderer);
}

// override from views::WidgetObserver
void RenderWindow::OnWidgetBoundsChanged(views::Widget* widget, 
                                         const gfx::Rect& new_bounds) {
  gfx::Rect rect = view()->GetContentsBounds();
  float aspect = (float)rect.width() / (float)rect.height();
  camera_.mutable_frustum()->set_aspect(aspect);
  camera_.Update();
}

void RenderWindow::OnWidgetDestroying(views::Widget* widget) {
  widget->RemoveObserver(this);
}
}  // namespace lord
