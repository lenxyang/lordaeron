#include "lordaeron/ui/camera_view.h"

namespace lord {
using namespace azer;
CameraView::CameraView(const Camera* camera) {
  object_.reset(new CameraObject(camera));
  object_->GetTransformHolder()->SetPosition(Vector3(-0.0f, 0.0f, 0.0f));

  Vector3 camera_pos(2.0f, 2.0f, 2.0f);
  Vector3 lookat(0.0f, 0.0f, 0.0f);
  Vector3 up(0.0f, 0.0f, 0.0f);
  overlay_camera_.reset(camera_pos, lookat, up);
}

void CameraView::Render() {
  Renderer* renderer = GetRenderer().get();
  object_->Update(overlay_camera_);
  renderer->Use();
  renderer->Clear(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
  renderer->ClearDepthAndStencil();
  object_->Render(renderer);
}
}  // namespace lord
