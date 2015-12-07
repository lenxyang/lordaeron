#include "lordaeron/interactive/picking_controller.h"

#include "azer/render/render.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/interactive/light_controller.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_bounding_volumn.h"
#include "lordaeron/ui/scene_render_window.h"


namespace lord {
using namespace azer;
BoundingVolumnPickingObject::BoundingVolumnPickingObject(SceneNode* node) {
  CHECK(node->type() == kSceneNode || node->type() == kMeshSceneNode);
  mesh_ = CreateBoundingBoxForSceneNode(node);
}

void BoundingVolumnPickingObject::Update(const FrameArgs& args) {
  mesh_->UpdateProviderParams(args);
}

void BoundingVolumnPickingObject::Render(Renderer* renderer) {
  mesh_->Render(renderer);
}

// class PickingController
PickingController::PickingController() {}

PickingController::~PickingController() {}
void PickingController::Update(const FrameArgs& args) {
  if (object_.get()) {
    object_->Update(args);
  }
}

void PickingController::Render(Renderer* renderer) {
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
      case kMeshSceneNode:
        object_.reset(new BoundingVolumnPickingObject(picking_node));
        break;
      case kLampSceneNode: {
        const Light* light = picking_node->mutable_data()->light();
        const Camera& camera = context_->window()->camera();
        if (light->type() == kPointLight) {
          object_.reset(new PointLightObject(&camera, picking_node));
        } else if (light->type() == kSpotLight) {
          object_.reset(new SpotLightObject(&camera, picking_node));
        } else if (light->type() == kDirectionalLight) {
          object_.reset(new DirLightObject(&camera, picking_node));
        }
        break;
      }
    }
  }
  return false;
}
}  // namespace lord
