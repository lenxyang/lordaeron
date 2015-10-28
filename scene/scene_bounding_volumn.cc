#include "lordaeron/scene/scene_bounding_volumn.h"

#include "base/logging.h"
#include "base/lazy_instance.h"
#include "azer/render/render.h"
#include "azer/render/util.h"

#include "lordaeron/context.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_context.h"
#include "lordaeron/effect/global_environemnt_params.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
using namespace azer;

namespace {
::base::LazyInstance<azer::EffectAdapterContext> adapter_context_
= LAZY_INSTANCE_INITIALIZER;
}

SceneBVProvider::SceneBVProvider(SceneNode* node)
    : node_(node),
      color_(Vector4(1.0f, 0.0f, 0.0f, 1.0f)) {
  if (!adapter_context_.Pointer()->LookupAdapter(
          SceneBVParamsAdapter::kAdapterKey)) {
    adapter_context_.Pointer()->RegisteAdapter(new SceneBVParamsAdapter);
  }
}

SceneBVProvider::~SceneBVProvider() {
}


void SceneBVProvider::UpdateParams(const azer::FrameArgs& args) {
  SceneContext* scene_ctx = node_->context();
  GlobalEnvironmentParams* gparams = scene_ctx->GetGlobalEnvironment();
  Vector3 vmin = node_->vmin();
  Vector3 vmax = node_->vmax();
  Vector3 scale1(vmax.x - vmin.x, vmax.y - vmin.y, vmax.z - vmin.z);
  Vector3 scale2 = node_->holder().scale();
  Vector3 scale = Vector3(scale1.x * scale2.x, 
                          scale1.y * scale2.y, 
                          scale1.z * scale2.z);
Vector3 pos = node_->holder().position();
  pv_ = gparams->camera()->GetProjViewMatrix();
  world_ = std::move(azer::Scale(scale));
  world_ = std::move(azer::Translate(pos)) * world_;
}

// class SceneBVParamsAdapter
const azer::EffectAdapterKey SceneBVParamsAdapter::kAdapterKey =
    std::make_pair(typeid(DiffuseEffect).name(),
                   typeid(SceneBVProvider).name());

azer::EffectAdapterKey SceneBVParamsAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(SceneBVProvider).name());
}

void SceneBVParamsAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  Context* ctx = Context::instance();
  const SceneBVProvider* box = dynamic_cast<const SceneBVProvider*>(params);
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(box && effect);

  effect->SetWorld(box->world_);
  effect->SetPV(box->pv_);
  effect->SetColor(box->color_);
  effect->SetDirLight(ctx->GetInternalLight());
}

azer::MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node) {
  Context* ctx = Context::instance();
  EffectPtr effect = ctx->GetEffect(DiffuseEffect::kEffectName);
  BoxObject* objptr = new BoxObject(effect->GetVertexDesc());
  
  azer::MeshPartPtr objpart = objptr->CreateObject(effect.get());
  azer::MeshPartPtr framepart = objptr->CreateFrameObject(effect.get());
  azer::MeshPtr mesh(new azer::Mesh(adapter_context_.Pointer()));
  mesh->AddMeshPart(objpart);
  mesh->AddMeshPart(framepart);
  scoped_refptr<SceneBVProvider> provider(new SceneBVProvider(node));
  mesh->AddProvider(provider);
  return mesh;
}
}  // namespace lord
