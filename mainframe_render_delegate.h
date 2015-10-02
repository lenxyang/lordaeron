#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"
#include "nelf/nelf.h"

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
  scoped_ptr<azer::CoordinateGrid> gridline_;
  DISALLOW_COPY_AND_ASSIGN(MainframeRenderDelegate);
};
}  // namespace lord
