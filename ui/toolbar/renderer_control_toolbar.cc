#include "lordaeron/ui/toolbar/renderer_control_toolbar.h"

#include "ui/views/controls/button/label_button_border.h"
#include "nelf/nelf.h"
#include "lordaeron/env.h"
#include "lordaeron/res/grit/common.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/interactive/fps_camera_controller.h"
#include "lordaeron/interactive/picking_controller.h"
#include "lordaeron/interactive/rotation_controller.h"
#include "lordaeron/interactive/scale_controller.h"
#include "lordaeron/interactive/translation_controller.h"

namespace lord {
RendererControlToolbar::RendererControlToolbar(nelf::MainFrame* mainframe) {
  LordEnv* context = LordEnv::instance();
  int32 toolbar_id1[] = {
    IDR_ICON_TOOLBAR_TREE,
    IDR_ICON_TOOLBAR_LAYERS,
    IDR_ICON_TOOLBAR_EXPLORER,
    IDR_ICON_TOOLBAR_MATERIAL,
    IDR_ICON_TOOLBAR_DIAMOND,
    IDR_ICON_TOOLBAR_MESH,
  };

  using views::BoxLayout;
  views::View* contents = new views::View;
  contents->SetLayoutManager(new BoxLayout(BoxLayout::kHorizontal, 0, 1, 0));
  for (uint32 i = 0; i < arraysize(toolbar_id1); ++i) {
    nelf::ResourceBundle* bundle = context->resource_bundle();
    int32 id = toolbar_id1[i];
    const gfx::ImageSkia* img = bundle->GetImageSkiaNamed(id);
    nelf::ToggleButton* btn = new nelf::ToggleButton(*img);
    btn->SetInsets(gfx::Insets(1, 1, 1, 1));
    btn->SetImageLabelSpacing(0);
    btn->set_listener(this);
    btn->set_tag(id);
    btn->SetMinSize(gfx::Size(32, 32));
    btn->SetTooltipText(::base::UTF8ToUTF16("This is tooltip"));
    contents->AddChildView(btn);
  }
  toolbar_ = new nelf::Toolbar(mainframe);
  toolbar_->SetContents(contents);
  toolbar_->Float();
}

RendererControlToolbar::~RendererControlToolbar() {
}

void RendererControlToolbar::ButtonPressed(views::Button* sender, 
                                           const ui::Event& event) {
}
}  // namespace lord
