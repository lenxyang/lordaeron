#include "lordaeron/ui/window/frame_window.h"

#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/render/coordinate.h"
#include "lordaeron/render/camera_overlay.h"
#include "lordaeron/ui/scene_tree_view.h"
#include "lordaeron/ui/scene_node_property_pane.h"
#include "lordaeron/ui/scene_node_inspector_pane.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/ui/toolbar/renderer_control_toolbar.h"

namespace lord {
using azer::Vector3;
using azer::Vector4;
namespace {
const Vector4 kGridLineColor = Vector4(0.663f, 0.663f, 0.663f, 1.0f);
const Vector4 kRenderBgColor = Vector4(0.427f, 0.427f, 0.427f, 1.0f);
}

FrameWindow::FrameWindow(const gfx::Rect& init_bounds)
    : RenderWindow(init_bounds),
      draw_gridline_(true) {
}

FrameWindow::FrameWindow(const gfx::Rect& init_bounds, 
                         nelf::RenderLoop* render_loop)
    : RenderWindow(init_bounds, render_loop),
      draw_gridline_(true) {
}

FrameWindow::~FrameWindow() {
}

bool FrameWindow::Initialize() {
  mutable_camera()->reset(Vector3(0.0f, 8.0f, 12.0f), Vector3(0.0f, 0.0f, 0.0f),
                          Vector3(0.0f, 1.0f, 0.0f));
  camera_overlay_.reset(new CameraOverlay(&camera()));
  gridline_.reset(new lord::CoordinateGrid(1.0f, 1.0f, 30));
  gridline_->SetColor(kGridLineColor);
  root_ = InitScene();
  interactive_.reset(new InteractiveContext(this, root_));

  OnInitUI();
  view()->AddEventListener(GetInteractive());
  RenderWindow::Initialize();
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  return true;
}

void FrameWindow::OnUpdate(const azer::FrameArgs& args) {
  azer::Renderer* renderer = window()->GetRenderer().get();
  interactive_->Update(args);
  RenderWindow::OnUpdate(args);
  gridline_->SetProjView(camera().GetProjViewMatrix());
  camera_overlay_->Update();
}

void FrameWindow::OnRender(const azer::FrameArgs& args) {
  azer::Renderer* renderer = window()->GetRenderer().get();
  renderer->Use();
  renderer->Clear(kRenderBgColor);
  renderer->ClearDepthAndStencil();
  OnRenderBegin(args, renderer);

  if (draw_gridline_) {
    gridline_->Render(renderer);
  }

  OnRenderFrame(args, renderer);
  renderer->Use();
  interactive_->Render(renderer);
  camera_overlay_->Render(renderer);

  OnRenderEnd(args, renderer);
}

void FrameWindow::OnInitUI() {
  gfx::Rect bounds(300, 360);
  nelf::TabbedWindow* wnd = CreateSceneTreeViewWindow(bounds, root(), this);
  wnd->Dock(nelf::kDockLeft);

  SceneNodeInspectorWindow* inspector = new SceneNodeInspectorWindow(bounds, this);
  inspector->Init();
  GetInteractive()->AddObserver(inspector->inspector_pane());
  inspector->Show();
  inspector->Dock(nelf::kDockLeft);
  ObjectControlToolbar* toolbar1 = new ObjectControlToolbar(this, GetInteractive());
  toolbar1->Float();
  toolbar1->Dock(0, 0);

  /*
  RendererControlToolbar* toolbar2 = new RendererControlToolbar(this);
  toolbar2->Float();
  toolbar2->Dock(0, 1);
  */
}
}  // namespace lord
