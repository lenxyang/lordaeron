#include "lordaeron/ui/frame_window.h"

#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/render/camera_overlay.h"
#include "lordaeron/ui/scene_tree_view.h"
#include "lordaeron/ui/scene_node_property_pane.h"
#include "lordaeron/ui/scene_node_inspector_pane.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"

namespace lord {
using azer::Vector3;
using azer::Vector4;
namespace {
const Vector4 kGridLineColor = Vector4(0.663f, 0.663f, 0.663f, 1.0f);
const Vector4 kRenderBgColor = Vector4(0.427f, 0.427f, 0.427f, 1.0f);
}

FrameWindow::FrameWindow(const gfx::Rect& init_bounds)
    : RenderWindow(init_bounds),
      draw_gridline_(false) {
}

FrameWindow::FrameWindow(const gfx::Rect& init_bounds, 
                         nelf::RenderLoop* render_loop)
    : RenderWindow(init_bounds, render_loop),
      draw_gridline_(false) {
}

bool FrameWindow::Initialize() {
  camera_overlay_.reset(new CameraOverlay(&camera()));
  gridline_.reset(new azer::CoordinateGrid(1.0f, 1.0f, 30));
  gridline_->SetXCoordColor(kGridLineColor);
  gridline_->SetZCoordColor(kGridLineColor);
  root_ = InitScene();
  interactive_.reset(new InteractiveContext(this, root_));

  OnInitUI();
  view()->AddEventListener(GetInteractive());
  RenderWindow::Initialize();
  return true;
}

void FrameWindow::OnUpdate(const azer::FrameArgs& args) {
  azer::Renderer* renderer = window()->GetRenderer().get();
  interactive_->Update(args);
  RenderWindow::OnUpdate(args);
  gridline_->Update(camera());
  camera_overlay_->Update();
}

void FrameWindow::OnRender(const azer::FrameArgs& args) {
  azer::Renderer* renderer = window()->GetRenderer().get();
  renderer->Use();
  renderer->Clear(kRenderBgColor);
  renderer->ClearDepthAndStencil();
  renderer->SetCullingMode(azer::kCullBack);
  renderer->EnableDepthTest(true);
  RenderWindow::OnRender(args);
  renderer->Use();
  if (draw_gridline_) {
    gridline_->Render(renderer);
  }
  interactive_->Render(renderer);
  camera_overlay_->Render(renderer);
}

void FrameWindow::OnInitUI() {
  gfx::Rect bounds(300, 360);
  nelf::TabbedWindow* wnd = CreateSceneTreeViewWindow(bounds, root(), this);
  wnd->Dock(nelf::kDockLeft);

  SceneNodeInspectorWindow* inspector = new SceneNodeInspectorWindow(bounds, this);
  inspector->Init();
  GetInteractive()->AddObserver(inspector->inspector_pane());
  inspector->Show();
  mutable_camera()->reset(Vector3(0.0f, 8.0f, 12.0f), Vector3(0.0f, 0.0f, 0.0f),
                          Vector3(0.0f, 1.0f, 0.0f));
  inspector->Dock(nelf::kDockLeft);
  ObjectControlToolbar* toolbar = new ObjectControlToolbar(this, GetInteractive());
}
}  // namespace lord
