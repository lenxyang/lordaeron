#include "lordaeron/ui/renderer_info_pane.h"

#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/views/background.h"
#include "ui/views/border.h"
#include "ui/views/round_rect_painter.h"
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

RendererInfoPane::RendererInfoPane() 
    : last_update_time_(0.0) {
  SkColor border_color = SkColorSetRGB(0, 0, 0);
  views::RoundRectPainter* painter = new views::RoundRectPainter(border_color, 8);
  scoped_ptr<views::Border> border(
      views::Border::CreateBorderPainter(painter, gfx::Insets(1, 1, 1, 1)).Pass());
  SetBorder(border.Pass());
  
  SkColor bgcolor = SkColorSetARGB(128, 0, 0, 0);
  set_background(views::Background::CreateSolidBackground(bgcolor));
  SetLayoutManager(new views::BoxLayout(views::BoxLayout::kVertical, 0, 0, 3));
  avg_fps_label_ = CreateLabel();
  cur_fps_label_ = CreateLabel();
  cull_mode_ = CreateLabel();
  depth_mode_ = CreateLabel();
  AddChildView(avg_fps_label_);
  AddChildView(cur_fps_label_);
  AddChildView(cull_mode_);
  AddChildView(depth_mode_);
  Layout();
}

gfx::Insets RendererInfoPane::GetInsets() const {
  gfx::Insets i = std::move(views::View::GetInsets());
  gfx::Insets insets(i.top() + 5,  
                     i.left() + 5,
                     i.bottom() + 5,
                     i.right() + 5);  
  return insets;
}

void RendererInfoPane::Update(azer::Renderer* renderer, 
                              const azer::FrameArgs& args) {
  using base::StringPrintf;
  using base::UTF8ToUTF16;
  if (args.time() - last_update_time_ > 0.5) {
    avg_fps_label_->SetText(UTF8ToUTF16(StringPrintf(
        "Average FPS: %f/s", args.total_average_fps())));
    cur_fps_label_->SetText(UTF8ToUTF16(StringPrintf(
        "Recent FPS: %f/s", args.recent_average_fps())));
    cull_mode_->SetText(UTF8ToUTF16(StringPrintf(
        "Cull Mode: %s", CullModeName(renderer->GetCullingMode()))));
    depth_mode_->SetText(UTF8ToUTF16(StringPrintf(
        "Depth Test: %s", (renderer->IsDepthTestEnable() ? "enabled" : "disabled"))));
    last_update_time_ = args.time();
  }
}
}  // namespace lord
