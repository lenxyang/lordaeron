#include "lordaeron/context.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "lordaeron/ui/nelf_context.h"

#include "lordaeron/effect/adapter/diffuse_effect_adapter.h"

namespace lord {
Context* Context::instance_ = NULL;;

Context* Context::instance() {
  CHECK(instance_);
  return instance_;
}

bool Context::InitContext(int argc, char* argv[]) {
  CHECK(!instance_);
  instance_ = new Context;
  return instance_->Init(argc, argv);
}

Context::Context() {
  internal_light_.dir = azer::Vector4(-0.6f, -0.6f, -0.2f, 0.0f);
  internal_light_.diffuse = azer::Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  internal_light_.ambient = azer::Vector4(0.2f, 0.2f, 0.2f, 1.0f);
}

Context::~Context() {
}

bool Context::Init(int argc, char* argv[]) {
  nelf_context_ = new NelfContext();
  if (!nelf_context_->Init(argc, argv)) {
    return false;
  }

  CHECK(LoadResourcePack(::base::FilePath(FILE_PATH_LITERAL(
      "out/dbg/lordaeron_resource.pak"))));

  azer::RenderSystem* rs = azer::RenderSystem::Current();

  using azer::Blending;
  Blending::Desc blend_desc;
  blend_desc.src = Blending::kSrcAlpha;
  blend_desc.dest = Blending::kSrcInvAlpha;
  blend_desc.oper = Blending::kAdd;
  blend_desc.src_alpha = Blending::kOne;
  blend_desc.dest_alpha = Blending::kZero;
  blend_desc.alpha_oper = Blending::kAdd;
  blend_desc.mask = Blending::kWriteColor;
  blending_ = rs->CreateBlending(blend_desc);
  CHECK(blending_.get());

  InitAdapterContext();
  return true;
}

void Context::InitAdapterContext() {
  azer::EffectAdapterContext* adapter_ctx = GetEffectAdapterContext();
  adapter_ctx->RegisteAdapter(new ColorEffectAdapter);
  adapter_ctx->RegisteAdapter(new SceneNodeColorEffectAdapter);
  adapter_ctx->RegisteAdapter(new GlobalEnvColorEffectAdapter);
}

bool Context::LoadResourcePack(const base::FilePath& path) {
  resource_bundle_.reset(new nelf::ResourceBundle);
  return resource_bundle_->Load(path);
}
}  // namespace lord
