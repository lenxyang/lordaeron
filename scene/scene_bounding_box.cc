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
::base::LazyInstance<EffectAdapterContext> adapter_context_ = LAZY_INSTANCE_INITIALIZER;
}
SceneBoundingBox::SceneBoundingBox(SceneNode* node)
    : node_(node),
      color_(Vector4(1.0f, 0.0f, 0.0f, 1.0f)) {
  scoped_ptr<ParamsAdapter> adapter(new ParamsAdapter);
  if (!adapter_context_.Pointer()->LookupAdapter(adapter->key())) {
    adapter_context_.Pointer()->RegisteAdapter(adapter.release());
  }

  Context* ctx = Context::instance();
  effect_ = ctx->GetEffect(DiffuseEffect::kEffectName);
  BoxObject* objptr = new BoxObject(effect_->GetVertexDesc());
  object_ = new RenderClosure(adapter_context_.Pointer());
  object_->SetVertexBuffer(objptr->GetVertexBuffer());
  object_->SetIndicesBuffer(objptr->GetIndicesBuffer());
  object_->AddProvider(EffectParamsProviderPtr(this));

  frame_ = new RenderClosure(adapter_context_.Pointer());
  frame_->SetVertexBuffer(objptr->GetVertexBuffer());
  frame_->SetIndicesBuffer(objptr->GetFrameIndicesBuffer());
  frame_->AddProvider(EffectParamsProviderPtr(this));
}

SceneBoundingBox::~SceneBoundingBox() {
  frame_ = NULL;
  object_ = NULL;
}

void SceneBoundingBox::UpdateProviderParams(const azer::FrameArgs& args) {
  UpdateParams(args);
}

void SceneBoundingBox::UpdateParams(const azer::FrameArgs& args) {
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

azer::EffectAdapterKey SceneBoundingBox::ParamsAdapter::key() const {
  return std::make_pair(typeid(DiffuseEffect).name(),
                        typeid(SceneBoundingBox).name());
}

void SceneBoundingBox::ParamsAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  Context* ctx = Context::instance();
  const SceneBoundingBox* box = dynamic_cast<const SceneBoundingBox*>(params);
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(box && effect);

  effect->SetWorld(box->world_);
  effect->SetPV(box->pv_);
  effect->SetColor(box->color_);
  effect->SetDirLight(ctx->GetInternalLight());
}

void SceneBoundingBox::Render(Renderer* renderer, Effect* effect) {
  ApplyParams(effect);

  Context* ctx = Context::instance();
  frame_->DrawIndex(renderer, effect, azer::kLineList);
  // BlendingPtr blending = ctx->GetDefaultBlending();
  // renderer->UseBlending(blending.get(), 0);
  object_->Draw(renderer, effect, azer::kTriangleList);
  // renderer->ResetBlending();
}
}  // namespace lord
