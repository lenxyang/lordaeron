#include "lordaeron/ui/toolbar/object_control_toolbar.h"

#include "nelf/nelf.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/interactive/fps_camera_controller.h"
#include "lordaeron/interactive/picking_controller.h"

namespace lord {
ObjectControlToolbar::ObjectControlToolbar(nelf::Mainframe* mainframe,
                                           InteractiveContext* ctx) 
    : interactive_(ctx) {
  Context* context = Context::instance();
  model_.reset(new nelf::SimpleToolbarModel);
  int32 toolbar_id1[] = {
    IDR_ICON_TOOLBAR_PICKING,
    IDR_ICON_TOOLBAR_MOVE,
    IDR_ICON_TOOLBAR_ROTATE,
    IDR_ICON_TOOLBAR_SCALE,
    IDR_ICON_TOOLBAR_FPS_CAMERA,
    IDR_ICON_TOOLBAR_LAMP,
  };
  for (int i = 0; i < arraysize(toolbar_id1); ++i) {
    nelf::ToolbarModel::Item item;
    item.id = toolbar_id1[i];
    item.icon = context->resource_bundle()->GetImageSkiaNamed(item.id);
    model_->AddItem(item);
  }

  nelf::ToggleToolbarContents* contents =
      new nelf::ToggleToolbarContents(model_.get());
  contents->set_listener(this);
  toolbar_ = new nelf::Toolbar(mainframe, contents);
  toolbar_->Float();
  toolbar_->Dock(0, 0);
}

ObjectControlToolbar::~ObjectControlToolbar() {
}

void ObjectControlToolbar::OnToolbarButtonPressed(views::Button* sender) {
  int32 id = sender->id();
  switch (id) {
    case IDR_ICON_TOOLBAR_PICKING:
      break;
    case IDR_ICON_TOOLBAR_MOVE:
      break;
    case IDR_ICON_TOOLBAR_ROTATE:
      break;
    case IDR_ICON_TOOLBAR_SCALE:
      break;
    case IDR_ICON_TOOLBAR_FPS_CAMERA: {
      using nelf::ToggleToolbarButton;
      ToggleToolbarButton* btn = dynamic_cast<ToggleToolbarButton*>(sender);
      DCHECK(btn);
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
