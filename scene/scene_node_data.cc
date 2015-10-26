#include "lordaeron/scene/scene_node_data.h"

#include "base/logging.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_context.h"
#include "lordaeron/effect/scene_node_params.h"
#include "lordaeron/effect/global_environemnt_params.h"

namespace lord {
SceneNodeData::SceneNodeData(SceneNode* node)
    : type_(SceneNode::kEmptyNode),
      node_(node) {
}

SceneNodeData::~SceneNodeData() {
}

azer::MeshPtr SceneNodeData::GetMesh() {
  DCHECK(mesh_.get());
  return mesh_;
}

void SceneNodeData::reset() {
  mesh_ = NULL;
  type_ = SceneNode::kEmptyNode;
  node_->UpdateBoundingHierarchy();
}

void SceneNodeData::AttachMesh(azer::MeshPtr mesh) {
  DCHECK(type_ == SceneNode::kEmptyNode);
  mesh_ = mesh;
  SceneNodeParamsPtr params(new SceneNodeParams(node_));
  mesh_->AddProvider(params);
  mesh_->AddProvider(node_->context()->GetGlobalEnvironment());
  type_ = SceneNode::kMeshNode;
  *node->mutable_vmin() = mesh->vmin();
  *node->mutable_vmax() = mesh->vmax();
  node_->UpdateBoundingHierarchy();
}

void SceneNodeData::AttachLight(LightPtr light) {
  DCHECK(light.get());
  DCHECK(type_ == SceneNode::kEmptyNode);
  light_ = light;
  type_ = SceneNode::kLampNode;
}
}  // namespace loard
