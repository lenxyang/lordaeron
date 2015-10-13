#include "lordaeron/scene/scene_node_data.h"

#include "base/logging.h"

namespace lord {
SceneNodeData::SceneNodeData(SceneNode::Type type)
    : type_(type) {
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
  if (mesh.get()) {
    type_ = SceneNode::kMeshNode;
  } else {
    type_ = SceneNode::kEmptyNode;
  }
}
}  // namespace loard
