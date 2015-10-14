#include "lordaeron/scene/scene_node_data.h"

#include "base/logging.h"
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
}

void SceneNodeData::AttachMesh(azer::MeshPtr mesh) {
  DCHECK(type_ == SceneNode::kMeshNode || type_ == SceneNode::kEmptyNode);
  mesh_ = mesh;
  mesh_->ResetCommonProvider();
  SceneNodeParamsPtr params(new SceneNodeParams(node_));
  mesh_->AddCommonProvider(params);
  mesh_->AddCommonProvider(context()->GetGlobalEnvironment());
  if (mesh.get()) {
    type_ = SceneNode::kMeshNode;
  } else {
    type_ = SceneNode::kEmptyNode;
  }
}
}  // namespace loard
