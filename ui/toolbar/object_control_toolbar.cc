#include "lordaeron/ui/toolbar/object_control_toolbar.h"

#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"

namespace lord {
ObjectControlToolbar::ObjectControlToolbar(nelf::Mainframe* mainframe) {
  Context* context = Context::instance();
  model_.reset(new nelf::SimpleToolbarModel);
  int32 toolbar_id1[] = {
    IDR_ICON_TOOLBAR_SELECT,
    IDR_ICON_TOOLBAR_MOVE,
    IDR_ICON_TOOLBAR_ROTATE,
    IDR_ICON_TOOLBAR_SCALE,
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

void ObjectControlToolbar::OnToolbarButtonPressed(int id) {
}
}  // namespace lord
