#include "lordaeron/mainframe_render_delegate.h"

#include "lordaeron/ui/renderer_info_pane.h"

namespace lord {
using namespace azer;

const Vector4 kGridLineColor = Vector4(0.663f, 0.663f, 0.663f, 1.0f);
const Vector4 kRenderBgColor = Vector4(0.427f, 0.427f, 0.427f, 1.0f);
MainframeRenderDelegate::MainframeRenderDelegate()
    : renderer_pane_(NULL) {
}

bool MainframeRenderDelegate::Initialize() { 
  light_.dir = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  light_.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  light_.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

  Vector3 camera_pos(0.0f, 1.0f, 5.0f);
  Vector3 lookat(0.0f, 1.0f, 0.0f);
  Vector3 up(0.0f, 1.0f, 0.0f);
  camera_.reset(camera_pos, lookat, up);
  camera_controller_.reset(new FPSCameraController(&camera_));
  view()->AddEventListener(camera_controller_.get());

  gridline_.reset(new CoordinateGrid(1.0f, 1.0f, 30));
  gridline_->SetXCoordColor(kGridLineColor);
  gridline_->SetZCoordColor(kGridLineColor);

  camera_overlay_.reset(new CameraOverlay(&camera_));
  this->window()->SetRenderUI(true);

  renderer_pane_ = new RendererInfoPane;
  renderer_pane_->SetBounds(10, 10, 180, 120);
  this->window()->AddChildView(renderer_pane_);
  return true;
}

void MainframeRenderDelegate::OnUpdate(const FrameArgs& args) {
  camera_controller_->Update(args);
  gridline_->Update(camera_);
  pv_ = camera_.GetProjViewMatrix();
  camera_overlay_->Update();
  Renderer* renderer = window()->GetRenderer().get();
  renderer_pane_->Update(renderer, args);
}

void MainframeRenderDelegate::OnRender(const FrameArgs& args) {
  Renderer* renderer = window()->GetRenderer().get();
  renderer->Use();
  renderer->Clear(kRenderBgColor);
  renderer->ClearDepthAndStencil();
  renderer->EnableDepthTest(true);
  gridline_->Render(renderer);

  camera_overlay_->Render(renderer);
}
}  // namespace lord
