#pragma once

#include "azer/render/camera.h"
#include "azer/render/util.h"
#include "nelf/nelf.h"

namespace lord {
class CameraView : public nelf::TextureRenderView {
 public:
  CameraView(const azer::Camera* camera);

  void Render() override;
 private:
  azer::Camera overlay_camera_;
  scoped_ptr<azer::CameraObject> object_;
  DISALLOW_COPY_AND_ASSIGN(CameraView);
};
}  // namespace lord
