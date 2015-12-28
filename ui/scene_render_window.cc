#include "lordaeron/ui/scene_render_window.h"

#include "ui/views/layout/layout_manager.h"

#include "lordaeron/env.h"
#include "lordaeron/interactive/fps_camera_controller.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/render/camera_overlay.h"
#include "lordaeron/ui/renderer_info_pane.h"

namespace lord {
using azer::Vector3;
using azer::Vector4;
namespace {
const Vector4 kGridLineColor = Vector4(0.663f, 0.663f, 0.663f, 1.0f);
const Vector4 kRenderBgColor = Vector4(0.427f, 0.427f, 0.427f, 1.0f);
}
SceneRenderWindow::SceneRenderWindow(const gfx::Rect& init_bounds)
    : nelf::MainFrame(init_bounds, LordEnv::instance()->GetNelfContext()),
      render_view_(NULL),
      draw_gridline_(false) {
}

SceneRenderWindow::SceneRenderWindow(const gfx::Rect& init_bounds, 
                                     nelf::RenderLoop* render_loop)
    : nelf::MainFrame(init_bounds, LordEnv::instance()->GetNelfContext()),
      render_view_(NULL),
      render_loop_(render_loop),
      draw_gridline_(NULL) {
}

SceneRenderWindow::~SceneRenderWindow() {
}

void SceneRenderWindow::InitRenderView() {
  if (!render_loop_.get()) {
    render_loop_ = new nelf::RenderLoop(this);
  }

  render_view_ = new nelf::RenderView(render_loop_.get(), this);
  SetLayoutManager(new views::FillLayout);
  AddChildView(render_view_);
  render_loop_->AddRenderView(render_view_);
}

void SceneRenderWindow::OnAfterWidgetInit() {
  nelf::MainFrame::OnAfterWidgetInit();
  InitRenderView();
}

// override from nelf::RenderDelegate
bool SceneRenderWindow::Initialize() {
  camera_overlay_.reset(new CameraOverlay(&camera_));

  renderer_pane_ = new RendererInfoPane;
  renderer_pane_->SetBounds(10, 10, 180, 120);
  this->window()->AddChildView(renderer_pane_);
  this->window()->SetRenderUI(true);
  window()->GetWidget()->AddObserver(this);

  Vector3 camera_pos(0.0f, 1.0f, 5.0f);
  Vector3 lookat(0.0f, 1.0f, 0.0f);
  Vector3 up(0.0f, 1.0f, 0.0f);
  camera_.reset(camera_pos, lookat, up);

  gridline_.reset(new azer::CoordinateGrid(1.0f, 1.0f, 30));
  gridline_->SetXCoordColor(kGridLineColor);
  gridline_->SetZCoordColor(kGridLineColor);

  interactive_.reset(new InteractiveContext(this));
  view()->AddEventListener(GetInteractive());
  root_ = OnInitScene();
  OnInitUI();
  return true;
}

void SceneRenderWindow::OnUpdate(const azer::FrameArgs& args) {
  azer::Renderer* renderer = window()->GetRenderer().get();
  interactive_->Update(args);
  OnUpdateFrame(args);
  gridline_->Update(camera_);
  renderer_pane_->Update(renderer, args);
  camera_overlay_->Update();
}

void SceneRenderWindow::OnRender(const azer::FrameArgs& args) {
  azer::Renderer* renderer = window()->GetRenderer().get();
  renderer->Use();
  renderer->Clear(kRenderBgColor);
  renderer->ClearDepthAndStencil();
  renderer->SetCullingMode(azer::kCullBack);
  renderer->EnableDepthTest(true);
  if (draw_gridline_) {
    gridline_->Render(renderer);
  }

  OnRenderFrame(args, renderer);

  renderer->Use();
  interactive_->Render(renderer);
  camera_overlay_->Render(renderer);
}

// override from views::WidgetObserver
void SceneRenderWindow::OnWidgetBoundsChanged(views::Widget* widget, 
                                              const gfx::Rect& new_bounds) {
  gfx::Rect rect = view()->GetContentsBounds();
  float aspect = (float)rect.width() / (float)rect.height();
  camera_.frustrum().set_aspect(aspect);
  camera_.Update();
}

void SceneRenderWindow::OnWidgetDestroying(views::Widget* widget) {
  widget->RemoveObserver(this);
}
}  // namespace lord
