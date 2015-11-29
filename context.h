#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"
#include "ui/gfx/image/image_skia.h"
#include "nelf/nelf.h"
#include "lordaeron/render/light.h"

namespace lord {
class Iconset;
class Context {
 public:
  static Context* instance();
  static bool InitContext(int argc, char* argv[]);

  nelf::Context* GetNelfContext() { return nelf_context_.get();}
  bool LoadResourcePack(const base::FilePath& path);
  nelf::ResourceBundle* resource_bundle() { return resource_bundle_.get();}
  gfx::ImageSkia GetIcon(int32 id);

  azer::EffectAdapterContext* GetEffectAdapterContext() { return &effect_context_;}
  const DirLight& GetInternalLight() const { return internal_light_;}
  azer::BlendingPtr GetDefaultBlending() { return blending_;}

  azer::Effect* GetEffect(const std::string& name);
 private:
  Context();
  ~Context();

  bool Init(int argc, char* argv[]);
  void InitEffects();
  void InitAdapterContext();

  scoped_ptr<nelf::ResourceBundle> resource_bundle_;
  scoped_ptr<Iconset> iconset_;
  azer::BlendingPtr blending_;
  DirLight internal_light_;
  nelf::ContextPtr nelf_context_;
  azer::EffectAdapterContext effect_context_;
  std::map<std::string, azer::EffectPtr> effects_;
  static Context* instance_;
  DISALLOW_COPY_AND_ASSIGN(Context);
};
}  // namespace lord
