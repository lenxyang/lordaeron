#include "lordaeron/interactive/picking_controller.h"

#include "azer/render/render.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_bounding_volumn.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/interactive/point_light_object.h"


namespace lord {
BoundingVolumnPickingObject::BoundingVolumnPickingObject(SceneNode* node) {
  CHECK(node->type() == kSceneNode || node->type() == kMeshSceneNode);
  mesh_ = CreateBoundingBoxForSceneNode(picking_node);
}

void BoundingVolumnPickingObject::Unpick() {
}

void BoundingVolumnPickingObject::Update(const azer::FrameArgs& args) {
  mesh_->UpdateProviderParams(args);
}

void BoundingVolumnPickingObject::Render(azer::Renderer* renderer) {
  mesh_->Render(renderer);
}

// class PickingController
PickingController::PickingController() {}

PickingController::~PickingController() {}
void PickingController::Update(const azer::FrameArgs& args) {
  if (object_.get()) {
    object_->Update(args);
  }
}

void PickingController::Render(azer::Renderer* renderer) {
  if (object_.get()) {
    object_->Render(renderer);
  }
}

void PickingController::OnLostFocus() {
}

bool PickingController::OnMousePressed(const ui::MouseEvent& event) {
  SceneNode* picking_node = context_->GetObjectFromLocation(event.location());
  context_->SetPickingNode(picking_node);
  if (picking_node) {
    switch (picking_node->type()) {
      case kSceneNode:
      case kMeshNode:
        object_.reset(new BoundingVolumnPickingObject(picking_node));
        break;
      case kLampNode:
        break;
    }
  }
  return false;
}
}  // namespace lord
