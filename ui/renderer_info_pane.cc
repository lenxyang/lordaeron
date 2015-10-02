#include "lordaeron/ui/renderer_info_pane.h"

#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/views/background.h"
#include "ui/views/border.h"
#include "ui/views/shadow_border.h"
#include "ui/views/layout/box_layout.h"


namespace lord {
using namespace azer;
const char* CullModeName(CullingMode mode) {
  switch (mode) {
    case kCullNone: return "Cull None";
    case kCullFront: return "Cull Front";
    case kCullBack: return "Cull Back";
    default: NOTREACHED(); return "";
  }
}

views::Label* CreateLabel() {
  views::Label* label = new views::Label;
  label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  label->SetAutoColorReadabilityEnabled(false);
  label->SetBackgroundColor(0x00000000);
  return label;
}

RendererInfoPane::RendererInfoPane() {
  scoped_ptr<views::Border> border(new views::ShadowBorder(8, 0x3D3C3AFF, 4, 4));
  SetBorder(border.Pass());
  set_background(views::Background::CreateSolidBackground(0x504A4B80));
  SetLayoutManager(new views::BoxLayout(views::BoxLayout::kVertical, 0, 0, 3));
  fps_label_ = CreateLabel();
  cull_mode_ = CreateLabel();
  depth_mode_ = CreateLabel();
  AddChildView(fps_label_);
  AddChildView(cull_mode_);
  AddChildView(depth_mode_);
  Layout();
}

void RendererInfoPane::Update(azer::Renderer* renderer, 
                              const azer::FrameArgs& args) {
  using base::StringPrintf;
  using base::UTF8ToUTF16;
  fps_label_->SetText(UTF8ToUTF16(StringPrintf(
      "FPS: %f/s", args.total_average_fps())));
  cull_mode_->SetText(UTF8ToUTF16(StringPrintf(
      "Cull Mode: %s", CullModeName(renderer->GetCullingMode()))));
  depth_mode_->SetText(UTF8ToUTF16(StringPrintf(
      "Depth Test: %s", (renderer->IsDepthTestEnable() ? "enabled" : "disabled"))));
}
}  // namespace lord
