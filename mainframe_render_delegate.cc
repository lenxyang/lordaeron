#include "lordaeron/mainframe_render_delegate.h"

namespace lord {
using namespace azer;
bool MainframeRenderDelegate::Initialize() { 
  light_.dir = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  light_.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  light_.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

  Vector3 camera_pos(0.0f, 1.0f, 5.0f);
  Vector3 lookat(0.0f, 1.0f, 0.0f);
  Vector3 up(0.0f, 1.0f, 0.0f);
  camera_.reset(camera_pos, lookat, up);
  camera_controller_.reset(new FPSCameraController(&camera_));

  gridline_.reset(new CoordinateGrid(1.0f, 1.0f, 100));
  gridline_->SetXCoordColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  gridline_->SetZCoordColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  return true;
}

void MainframeRenderDelegate::OnUpdate(const FrameArgs& args) {
  gridline_->Update(camera_);
}

void MainframeRenderDelegate::OnRender(const FrameArgs& args) {
  Renderer* renderer = window()->GetRenderer().get();
  renderer->Use();
  renderer->Clear(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  renderer->SetCullingMode(kCullBack);
  renderer->EnableDepthTest(true);
  gridline_->Render(renderer);
}
}  // namespace lord
