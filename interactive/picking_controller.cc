#include "lordaeron/interactive/picking_controller.h"

#include "azer/render/render.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/scene_render_window.h"


namespace lord {
using namespace azer;

// class PickingController
PickingController::PickingController() {}

PickingController::~PickingController() {}
void PickingController::Update(const FrameArgs& args) {
}

void PickingController::Render(Renderer* renderer) {
}

void PickingController::OnLostFocus() {
}

bool PickingController::OnMousePressed(const ui::MouseEvent& event) {
  SceneNode* node = context_->GetObjectFromLocation(event.location());
  context_->SetPickingNode(node);
  return false;
}
}  // namespace lord
