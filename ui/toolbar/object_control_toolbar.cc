#include "lordaeron/ui/toolbar/object_control_toolbar.h"

#include "ui/views/controls/button/label_button_border.h"
#include "nelf/nelf.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/interactive/fps_camera_controller.h"
#include "lordaeron/interactive/picking_controller.h"
#include "lordaeron/interactive/rotation_controller.h"

namespace lord {
ObjectControlToolbar::ObjectControlToolbar(nelf::Mainframe* mainframe,
                                           InteractiveContext* ctx) 
    : interactive_(ctx) {
  Context* context = Context::instance();
  int32 toolbar_id1[] = {
    IDR_ICON_TOOLBAR_PICKING,
    IDR_ICON_TOOLBAR_MOVE,
    IDR_ICON_TOOLBAR_ROTATE,
    IDR_ICON_TOOLBAR_SCALE,
    IDR_ICON_TOOLBAR_FPS_CAMERA,
    IDR_ICON_TOOLBAR_LAMP,
  };
  
  nelf::GroupButtonPane* contents = new nelf::GroupButtonPane;
  for (uint32 i = 0; i < arraysize(toolbar_id1); ++i) {
    nelf::ResourceBundle* bundle = context->resource_bundle();
    int32 id = toolbar_id1[i];
    const gfx::ImageSkia* img = bundle->GetImageSkiaNamed(id);
    nelf::GroupButton* btn = new nelf::GroupButton(*img);
    btn->set_listener(this);
    btn->set_tag(id);

    btn->SetImageLabelSpacing(0);
    scoped_ptr<views::LabelButtonBorder> border(
        new views::LabelButtonBorder(views::Button::STYLE_TEXTBUTTON));
    border->set_insets(gfx::Insets(0, 0, 0, 0));
    btn->SetBorder(border.Pass());
    contents->AddGroupButton(btn);
  }
  toolbar_ = new nelf::Toolbar(mainframe, contents);
  toolbar_->Float();
  toolbar_->Dock(0, 0);
}

ObjectControlToolbar::~ObjectControlToolbar() {
}

void ObjectControlToolbar::ButtonPressed(views::Button* sender,
                                         const ui::Event& event) {
  using nelf::GroupButton;
  int32 id = sender->tag();
  GroupButton* btn = dynamic_cast<GroupButton*>(sender);
  DCHECK(btn);
  switch (id) {
    case IDR_ICON_TOOLBAR_PICKING: {
      if (btn->pressed()) {
        scoped_ptr<PickingController> controller(new PickingController());
        interactive_->SetController(controller.Pass());
      } else {
        interactive_->ResetController();
      }
    }
      break;
    case IDR_ICON_TOOLBAR_MOVE:
      break;
    case IDR_ICON_TOOLBAR_ROTATE: {
      if (btn->pressed()) {
        scoped_ptr<RotationController> controller(new RotationController());
        interactive_->SetController(controller.Pass());
      } else {
        interactive_->ResetController();
      }
      break;
    }
    case IDR_ICON_TOOLBAR_SCALE:
      break;
    case IDR_ICON_TOOLBAR_FPS_CAMERA: {
      if (btn->pressed()) {
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
