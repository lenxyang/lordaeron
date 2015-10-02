#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"

namespace lord {
class CameraOverlay {
 public:
  CameraOverlay(const azer::Camera* camera);

  void SetShowBounds(const gfx::RectF& bounds);
  void Update();
  void Render(azer::Renderer* renderer);
 private:
  azer::Matrix4 pvw_;
  azer::Matrix4 world_;
  azer::OverlayPtr overlay_;
  azer::RendererPtr renderer_;
  azer::Renderer::Viewport viewport_;
  scoped_ptr<azer::AxesFrames> object_;
  const azer::Camera* camera_;
  DISALLOW_COPY_AND_ASSIGN(CameraOverlay);
};

}  // namespace lord
