#include "lordaeron/interactive/picking_controller.h"

#include "azer/render/render.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_bounding_volumn.h"
#include "lordaeron/ui/scene_render_window.h"


namespace lord {

PickingController::PickingController() {
}

PickingController::~PickingController() {
}

void PickingController::Update(const azer::FrameArgs& args) {
  if (mesh_.get()) {
    mesh_->UpdateProviderParams(args);
  }
}

void PickingController::Render(azer::Renderer* renderer) {
  if (mesh_.get()) {
    // bool enable_depth = renderer->IsDepthTestEnable();
    // enderer->EnableDepthTest(false);
    mesh_->Render(renderer);
    // renderer->EnableDepthTest(enable_depth);
  }
}

void PickingController::OnLostFocus() {
}

bool PickingController::OnMousePressed(const ui::MouseEvent& event) {
  SceneNode* picking_node = context_->GetObjectFromLocation(event.location());
  context_->SetPickingNode(picking_node);
  if (picking_node) {
    mesh_ = CreateBoundingBoxForSceneNode(picking_node);
  }
  return false;
}
}  // namespace lord
