#include "lordaeron/env.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "lordaeron/ui/nelf_context.h"
#include "lordaeron/ui/iconset.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/effect/normal_line_effect.h"
#include "lordaeron/resource/resource_loader.h"
#include "lordaeron/interactive/light_controller.h"
#include "lordaeron/scene/ui_scene_render.h"

namespace lord {
LordEnv* LordEnv::instance_ = NULL;;

LordEnv* LordEnv::instance() {
  CHECK(instance_);
  return instance_;
}

bool LordEnv::InitEnv(int argc, char* argv[]) {
  CHECK(!instance_);
  instance_ = new LordEnv;
  return instance_->Init(argc, argv);
}

LordEnv::LordEnv() {
  internal_light_.direction = azer::Vector3(-0.6f, -0.6f, -0.2f);
  internal_light_.diffuse = azer::Vector4(0.4f, 0.4f, 0.4f, 1.0f);
  internal_light_.ambient = azer::Vector4(0.2f, 0.2f, 0.2f, 1.0f);
  internal_light_.specular = azer::Vector4(0.1f, 0.1f, 0.1f, 1.0f);
  internal_light_.enable = 1.0f;
}

LordEnv::~LordEnv() {
}

bool LordEnv::Init(int argc, char* argv[]) {
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
  iconset_.reset(new Iconset(this));

  InitEffects();
  return true;
}

void LordEnv::SetFileSystem(scoped_ptr<azer::FileSystem> filesystem) {
  file_system_ = filesystem.Pass();
  resource_loader_.reset(new ResourceLoader(file_system_.get()));
}

void LordEnv::InitEffects() {
  effects_.insert(std::make_pair(DiffuseEffect::kEffectName, CreateDiffuseEffect()));
  effects_.insert(std::make_pair(NormalLineEffect::kEffectName, CreateNormalLineEffect()));
}

azer::Effect* LordEnv::GetEffect(const std::string& name) {
  auto iter = effects_.find(name);
  if (iter != effects_.end())
    return iter->second.get();
  else
    return NULL;
}

gfx::ImageSkia LordEnv::GetIcon(int32 id) {
  return iconset_->GetIcon(id);
}

void LordEnv::InitAdapterContext() {
  effect_context_.RegisteAdapter(new DiffuseColorEffectAdapter);
  effect_context_.RegisteAdapter(new RenderNodeDiffuseEffectAdapter);
  effect_context_.RegisteAdapter(new RenderEnvNodeDiffuseEffectAdapter);
  effect_context_.RegisteAdapter(new LightControllerColorEffectAdapter);
  effect_context_.RegisteAdapter(new LightControllerEffectAdapter);
  effect_context_.RegisteAdapter(new LordSceneBVParamsAdapter);
  effect_context_.RegisteAdapter(new NormalLineEffectAdapter);
}

bool LordEnv::LoadResourcePack(const base::FilePath& path) {
  resource_bundle_.reset(new nelf::ResourceBundle);
  return resource_bundle_->Load(path);
}
}  // namespace lord
