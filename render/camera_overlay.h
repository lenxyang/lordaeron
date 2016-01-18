#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"

namespace lord {
class AxesFrame;

class CameraOverlay {
 public:
  CameraOverlay(const azer::Camera* camera);
  ~CameraOverlay();

  void SetShowBounds(const gfx::RectF& bounds);
  void Update();
  void Render(azer::Renderer* renderer);
 private:
  azer::Matrix4 world_;
  azer::OverlayPtr overlay_;
  azer::RendererPtr renderer_;
  azer::Viewport viewport_;
  scoped_ptr<AxesFrame> object_;
  const azer::Camera* camera_;
  DISALLOW_COPY_AND_ASSIGN(CameraOverlay);
};

}  // namespace lord
