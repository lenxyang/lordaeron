#include "lordaeron/render/light_mesh.h"

#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using namespace azer;
MeshPtr CreatePointLightMesh(SceneNode* node) {
  MeshPtr mesh = new Mesh;
  DiffuseEffectPtr effect = CreateDiffuseEffect();
  GeometryObjectPtr obj = new SphereObject(effect->GetVertexDesc());
  MeshPartPtr part = obj->CreateObject(effect.get());
  mesh->AddMeshPart(part.get());
  return mesh;
}

MeshPtr CreateSpotLightMesh(SceneNode* node) {
  return MeshPtr();
}

MeshPtr CreateDirectionalLightMesh(SceneNode* node) {
  return MeshPtr();
}
}  // namespace lord
