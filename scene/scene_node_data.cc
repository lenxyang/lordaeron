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
  node->AddObserver(this);
}

SceneNodeData::~SceneNodeData() {
  // neednot to Remove observer, because it will be release by SceneNode
  // node_->RemoveObserver(this);
}

MeshPtr SceneNodeData::GetMesh() {
  DCHECK(mesh_.get());
  return mesh_;
}

void SceneNodeData::reset() {
  if (light_.get()) {
    light_ = NULL;
  }

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
  
  node_->SetMin(light_mesh->vmin());
  node_->SetMax(light_mesh->vmax());
}


void SceneNodeData::OnSceneNodeOrientationChanged(
    SceneNode* node, const azer::Quaternion& prev_orient) {
  if (node_->type() == SceneNode::kLampNode && light_->type() == kDirectionalLight) {
    Matrix4 rotation = std::move(node->orientation().ToMatrix());
    Vector4 newdir = rotation * Vector4(0.0f, 0.0f, 1.0f, 0.0f);
    light_->mutable_dir_light()->direction = Vector3(newdir.x, newdir.y, newdir.z);;
  }
}
}  // namespace loard
