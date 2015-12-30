#include "lordaeron/ui/toolbar/object_control_toolbar.h"

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
ObjectControlToolbar::ObjectControlToolbar(nelf::MainFrame* mainframe,
                                           InteractiveContext* ctx) 
    : nelf::Toolbar(mainframe),
      interactive_(ctx) {
  LordEnv* context = LordEnv::instance();
  int32 toolbar_id1[] = {
    IDR_ICON_TOOLBAR_PICKING,
    IDR_ICON_TOOLBAR_MOVE,
    IDR_ICON_TOOLBAR_ROTATE,
    IDR_ICON_TOOLBAR_SCALE,
    IDR_ICON_TOOLBAR_FPS_CAMERA,
    IDR_ICON_TOOLBAR_LAMP,
  };
  
  nelf::GroupButtonPane* contents = new nelf::GroupButtonPane;
  contents->set_delegate(this);
  for (uint32 i = 0; i < arraysize(toolbar_id1); ++i) {
    nelf::ResourceBundle* bundle = context->resource_bundle();
    int32 id = toolbar_id1[i];
    const gfx::ImageSkia* img = bundle->GetImageSkiaNamed(id);
    nelf::ToggleButton* btn = new nelf::ToggleButton(*img);
    btn->SetInsets(gfx::Insets(1, 1, 1, 1));
    btn->SetImageLabelSpacing(0);
    btn->set_tag(id);
    btn->SetMinSize(gfx::Size(32, 32));
    btn->SetTooltipText(::base::UTF8ToUTF16("This is tooltip"));
    contents->AddToggleButton(btn);
  }
  SetContents(contents);
}

ObjectControlToolbar::~ObjectControlToolbar() {
}

void ObjectControlToolbar::OnToggleButtonStateChanged(nelf::ToggleButton* btn) {
  if (!btn->toggled())
    return;

  using nelf::ToggleButton;
  int32 id = btn->tag();
  DCHECK(btn);
  switch (id) {
    case IDR_ICON_TOOLBAR_PICKING: {
      if (btn->toggled()) {
        scoped_ptr<PickingController> controller(new PickingController());
        interactive_->SetController(controller.Pass());
      } else {
        interactive_->ResetController();
      }
    }
      break;
    case IDR_ICON_TOOLBAR_MOVE: {
      if (btn->toggled()) {
        scoped_ptr<TranslationController> controller(new TranslationController());
        interactive_->SetController(controller.Pass());
      } else {
        interactive_->ResetController();
      }
      break;
    }
    case IDR_ICON_TOOLBAR_ROTATE: {
      if (btn->toggled()) {
        scoped_ptr<RotationController> controller(new RotationController());
        interactive_->SetController(controller.Pass());
      } else {
        interactive_->ResetController();
      }
      break;
    }
    case IDR_ICON_TOOLBAR_SCALE:
       {
      if (btn->toggled()) {
        scoped_ptr<ScaleController> controller(new ScaleController());
        interactive_->SetController(controller.Pass());
      } else {
        interactive_->ResetController();
      }
      break;
    }
    case IDR_ICON_TOOLBAR_FPS_CAMERA: {
      if (btn->toggled()) {
        scoped_ptr<FPSCameraController> controller(new FPSCameraController());
        interactive_->SetController(controller.Pass());
      } else {
        interactive_->ResetController();
      }
      break;
    }
    case IDR_ICON_TOOLBAR_LAMP:
      break;
    default:
      break;
  }
}
}  // namespace lord
