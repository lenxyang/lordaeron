#include "lordaeron/render/light_mesh.h"

#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using namespace azer;

MeshPtr CreatePointLightMesh() {
  MeshPtr mesh = new Mesh;
  DiffuseEffectPtr effect = CreateDiffuseEffect();
  GeometryObjectPtr obj = new SphereObject(effect->GetVertexDesc());
  MeshPartPtr part = obj->CreateObject(effect.get());
  mesh->AddMeshPart(part.get());
  return mesh;
}

MeshPtr CreateSpotLightMesh() {
  return MeshPtr();
}

MeshPtr CreateDirectionalLightMesh() {
  return MeshPtr();
}

LightColorProvider::LightColorProvider(Light* light) 
    : light_(light) {
}

LightColorProvider::~LightColorProvider() {
}

void LightColorProvider::UpdateParams(const FrameArgs& args) {
}

const Vector4& LightColorProvider::color() const { 
  return light_->ambient();
}

// class LightColorDiffuseEffectAdapter
LightColorDiffuseEffectAdapter::LightColorDiffuseEffectAdapter() {
}

EffectAdapterKey LightColorDiffuseEffectAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(LightColorProvider).name());
}

void LightColorDiffuseEffectAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(effect);
  const LightColorProvider* provider = dynamic_cast<const LightColorProvider*>(params);
  DCHECK(provider);
  effect->SetColor(provider->color());
}
}  // namespace lord
