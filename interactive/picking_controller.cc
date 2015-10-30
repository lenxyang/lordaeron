#include "lordaeron/interactive/picking_controller.h"

#include "azer/math/ray.h"
#include "azer/render/render.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {

namespace {
using namespace azer;
Vector4 CalcClickedWorldPos(const gfx::Point& pt, const gfx::Size& size,
                            Camera* camera) {
  float x = 2 * (float)pt.x() / (float)size.width() - 1.0f;
  float y = -2 * (float)pt.y() / (float)size.height() + 1.0f;
  Vector4 pos(x, y, 1.0f, 1.0f);
  Matrix4 ins = camera->GetViewMatrix().InverseCopy();
  return ins * pos;
}

Ray GetPickingRay(const gfx::Point& pt, const gfx::Size& size,
                      Camera* camera) {
  Vector4 pos = Vector4(camera->holder().position(), 1.0f);
  Vector4 pos2 = CalcClickedWorldPos(pt, size, camera);
  Vector4 dir = pos - pos2;
  return Ray(Vector3(pos.x, pos.y, pos.z), Vector3(dir.x, dir.y, dir.z));
}
// another method
// reference 3D Game engine design(2nd edition) 8.4, Object Picking
}  // namespace azer

PickingController::PickingController()
    : picking_node_(NULL) {
}

PickingController::~PickingController() {
}


void PickingController::OnOperationStart(InteractiveContext* ctx) {
}

bool PickingController::OnKeyPressed(const ui::KeyEvent& event) {
  return false;
}
}  // namespace lord
