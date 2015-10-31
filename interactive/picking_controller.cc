#include "lordaeron/interactive/picking_controller.h"

#include "azer/math/ray.h"
#include "azer/render/render.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_picking.h"
#include "lordaeron/scene/scene_bounding_volumn.h"
#include "lordaeron/ui/scene_render_window.h"


namespace lord {

namespace {
using namespace azer;
Vector4 CalcClickedWorldPos(const gfx::Point& pt, const gfx::Size& size,
                            const Camera* camera) {
  float x = 2 * (float)pt.x() / (float)size.width() - 1.0f;
  float y = -2 * (float)pt.y() / (float)size.height() + 1.0f;
  Vector4 pos(x, y, 0.0f, 1.0f);

  const Matrix4& proj = camera->frustrum().projection();
  const Matrix4& view = camera->GetViewMatrix();
  Matrix4 mat = std::move(view.InverseCopy() * proj.InverseCopy());
  
  pos = mat * pos;
  pos /= pos.w;
  return pos;
}

Ray GetPickingRay(const gfx::Point& pt, const gfx::Size& size,
                  const Camera* camera) {
  Vector4 pos = Vector4(camera->holder().position(), 1.0f);
  Vector4 pos2 = CalcClickedWorldPos(pt, size, camera);
  Vector4 dir = pos - pos2;
  Vector3 nd(dir.x, dir.y, dir.z);
  nd.Normalize(); 
  return Ray(Vector3(pos.x, pos.y, pos.z), nd);
}
// another method
// reference 3D Game engine design(2nd edition) 8.4, Object Picking
}  // namespace azer

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
  azer::Ray ray = GetPickingRay(event.location(),
                                context_->window()->GetContentsBounds().size(),
                                &context_->window()->camera());
  SceneNodePickHelper helper(&ray);
  SceneNodeTraverse traverser(&helper);
  traverser.Traverse(context_->root());
  SceneNode* picking_node = helper.GetPickingNode();
  context_->SetPickingNode(picking_node);
  if (picking_node) {
    mesh_ = CreateBoundingBoxForSceneNode(picking_node);
  }
  return false;
}
}  // namespace lord
