#include "lordaeron/scene/scene_node_picking.h"

#include "lordaeron/scene/scene_node.h"
#include "azer/math/math.h"
#include "azer/render/render.h"

namespace lord {
SceneNodePickHelper::SceneNodePickHelper(azer::Ray* ray)
    : ray_(ray),
      picking_node_(NULL) {
}

SceneNodePickHelper::~SceneNodePickHelper() {
}

SceneNode* SceneNodePickHelper::GetPickingNode() {
  return picking_node_;
}

void SceneNodePickHelper::OnTraverseBegin(SceneNode* root) {
}

bool SceneNodePickHelper::OnTraverseNodeEnter(SceneNode* node) {
  if (picking_node_)
    return false;

  if (node->pickable())
    return false;

  using namespace azer;
  Vector3 vmin = node->vmin();
  Vector3 vmax = node->vmax();
  Vector3 other = vmin;
  other.x = vmax.x;
  other.y = vmax.y;
  Plane plane(vmin, vmax, other);
  Vector3 pt = plane.intersect(*ray_);
  if (pt.x >= vmin.x && pt.y >= vmin.y && pt.z >= vmin.z
      && pt.x <= vmax.x && pt.y <= vmax.y && pt.z <= vmax.z) {
    if (node->has_child()) {
      return true;
    } else {
      picking_node_ = node;
      return false;
    }
  } else {
    return false;
  }
}

void SceneNodePickHelper::OnTraverseNodeExit(SceneNode* node) {
}

void SceneNodePickHelper::OnTraverseEnd() {
}

}  // namespace lord
