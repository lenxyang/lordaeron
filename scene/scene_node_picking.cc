#include "lordaeron/scene/scene_node_picking.h"

#include "azer/math/math.h"
#include "azer/render/render.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/util/axis_aligned_bounding_box.h"

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
  AxisAlignedBoundingBox aabb(vmin, vmax);
  if (aabb.IsIntersect(*ray_)) {
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
