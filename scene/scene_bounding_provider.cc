#include "lordaeron/scene/scene_bounding_box.h"

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
::base::LazyInstance<EffectAdapterContext> adapter_context_
= LAZY_INSTANCE_INITIALIZER;
}
SceneVBProvider::SceneVBProvider(SceneNode* node)
    : node_(node),
      color_(Vector4(1.0f, 0.0f, 0.0f, 1.0f)) {
  scoped_ptr<ParamsAdapter> adapter(new ParamsAdapter);
  if (!adapter_context_.Pointer()->LookupAdapter(adapter->key())) {
    adapter_context_.Pointer()->RegisteAdapter(adapter.release());
  }
}

SceneVBProvider::~SceneVBProvider() {
}


void SceneVBProvider::UpdateParams(const azer::FrameArgs& args) {
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

azer::EffectAdapterKey SceneVBProvider::ParamsAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(SceneVBProvider).name());
}

void SceneVBProvider::ParamsAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  Context* ctx = Context::instance();
  const SceneVBProvider* box = dynamic_cast<const SceneVBProvider*>(params);
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(box && effect);

  effect->SetWorld(box->world_);
  effect->SetPV(box->pv_);
  effect->SetColor(box->color_);
  effect->SetDirLight(ctx->GetInternalLight());
}

}  // namespace lord
