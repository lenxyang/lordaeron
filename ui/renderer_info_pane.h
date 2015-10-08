#pragma once

#include "nelf/nelf.h"
#include "ui/views/view.h"

namespace lord {
class RendererInfoPane : public views::View {
 public:
  RendererInfoPane();

  void Update(azer::Renderer* renderer, const azer::FrameArgs& args);
 private:
  views::Label* fps_label_;
  views::Label* cull_mode_;
  views::Label* depth_mode_;
  double last_update_time_;
  DISALLOW_COPY_AND_ASSIGN(RendererInfoPane);
};
}  // namespace lord
