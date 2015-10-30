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

// namespace
namespace {
class BoundingBoxEntity : public azer::Entity {
 public:
 private:
};
}

SceneBVProvider::SceneBVProvider(SceneNode* node)
    : node_(node),
      color_(Vector4(1.0f, 0.0f, 0.0f, 0.3f)) {
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
  Vector3 center = (vmin + vmax) * 0.5f;
  Vector3 scale(vmax.x - vmin.x, vmax.y - vmin.y, vmax.z - vmin.z);
  pv_ = gparams->camera()->GetProjViewMatrix();
  world_ = std::move(azer::Scale(scale));
  world_ = std::move(azer::Translate(center)) * world_;
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
  BlendingPtr blending = ctx->GetDefaultBlending();
  objpart->SetBlending(blending);
  azer::MeshPartPtr framepart = objptr->CreateFrameObject(effect.get());
  azer::MeshPtr mesh(new azer::Mesh(adapter_context_.Pointer()));
  mesh->AddMeshPart(objpart);
  mesh->AddMeshPart(framepart);
  scoped_refptr<SceneBVProvider> provider(new SceneBVProvider(node));
  mesh->AddProvider(provider);
  return mesh;
}
}  // namespace lord
