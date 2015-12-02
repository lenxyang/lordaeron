#include "lordaeron/scene/scene_node_data.h"

#include "base/logging.h"
#include "lordaeron/scene/scene_node.h"
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

Mesh* SceneNodeData::GetMesh() {
  DCHECK(mesh_.get());
  return mesh_.get();
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

Light* SceneNodeData::light() { 
  return light_.get();
}

void SceneNodeData::AttachMesh(MeshPtr mesh) {
  DCHECK(type_ == SceneNode::kEmptyNode);
  mesh_ = mesh;
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
