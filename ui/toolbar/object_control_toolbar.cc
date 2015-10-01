#include "lordaeron/ui/toolbar/object_control_toolbar.h"

#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"

namespace lord {
ObjectControlToolbar::ObjectControlToolbar(nelf::Mainframe* mainframe) {
  Context* context = Context::instance();
  model_.reset(new nelf::SimpleToolbarModel);
  int32 toolbar_id1[] = {
    IDR_ICON32_SELECT,
    IDR_ICON32_MOVE,
    IDR_ICON32_ROTATE,
    IDR_ICON32_SCALE,
  };
  for (int i = 0; i < arraysize(toolbar_id1); ++i) {
    nelf::ToolbarModel::Item item;
    item.id = toolbar_id1[i];
    item.icon = context->resource_bundle()->GetImageSkiaNamed(item.id);
    model_->AddItem(item);
  }

  toolbar_ = new nelf::Toolbar(mainframe, model_.get(), this);
  toolbar_->Float();
}

ObjectControlToolbar::~ObjectControlToolbar() {
}

void ObjectControlToolbar::OnToolbarButtonPressed(int id) {
}
}  // namespace lord
