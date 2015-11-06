#include "lordaeron/scene/scene_node_data.h"

#include "base/logging.h"
#include "lordaeron/effect/scene_node_params.h"
#include "lordaeron/effect/global_environemnt_params.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_context.h"
#include "lordaeron/render/light_mesh.h"

namespace lord {
using namespace azer;
SceneNodeData::SceneNodeData(SceneNode* node)
    : type_(SceneNode::kEmptyNode),
      node_(node) {
}

SceneNodeData::~SceneNodeData() {
}

MeshPtr SceneNodeData::GetMesh() {
  DCHECK(mesh_.get());
  return mesh_;
}

void SceneNodeData::reset() {
  mesh_ = NULL;
  type_ = SceneNode::kEmptyNode;
  node_->SetMin(Vector3(0.0f, 0.0f, 0.0f));
  node_->SetMax(Vector3(0.0f, 0.0f, 0.0f));
}

void SceneNodeData::AttachMesh(MeshPtr mesh) {
  DCHECK(type_ == SceneNode::kEmptyNode);
  mesh_ = mesh;
  SceneNodeParamsPtr params(new SceneNodeParams(node_));
  mesh_->AddProvider(params);
  mesh_->AddProvider(node_->context()->GetGlobalEnvironment());
  type_ = SceneNode::kMeshNode;
  node_->SetMin(mesh->vmin());
  node_->SetMax(mesh->vmax());
}

void SceneNodeData::AttachLight(LightPtr light) {
  DCHECK(light.get());
  DCHECK(type_ == SceneNode::kEmptyNode);
  light_ = light;
  type_ = SceneNode::kLampNode;

  // set mesh
  Mesh* light_mesh = light_->GetLightMesh();
  DCHECK(light_mesh);
  light_mesh->AddProvider(new LightColorProvider(light_.get()));
  SceneNodeParamsPtr params(new SceneNodeParams(node_));
  light_mesh->AddProvider(params);
  light_mesh->AddProvider(node_->context()->GetGlobalEnvironment());
}
}  // namespace loard
