#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"
#include "nelf/nelf.h"
#include "lordaeron/render/camera_overlay.h"
#include "lordaeron/render/rotate_controller_object.h"

namespace lord {
class MainframeRenderDelegate : public nelf::RenderDelegate {
 public:
  MainframeRenderDelegate();
  virtual bool Initialize() override;
  virtual void OnUpdate(const azer::FrameArgs& args) override;
  virtual void OnRender(const azer::FrameArgs& args) override;

  // attributes
  const azer::Camera& camera() const { return camera_;}
 private:

  azer::Camera camera_;
  azer::DirLight light_;
  azer::Matrix4 pv_;
  scoped_ptr<CameraOverlay> camera_overlay_;
  scoped_ptr<azer::CoordinateGrid> gridline_;
  scoped_ptr<azer::FPSCameraController> camera_controller_;
  scoped_ptr<RotateControllerObject> rotate_controller_object_;
  DISALLOW_COPY_AND_ASSIGN(MainframeRenderDelegate);
};
}  // namespace lord
