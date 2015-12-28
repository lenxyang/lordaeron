#pragma once

#include "azer/base/file_system.h"
#include "azer/render/render.h"
#include "azer/render/util.h"
#include "ui/gfx/image/image_skia.h"
#include "nelf/nelf.h"
#include "lordaeron/effect/light.h"

namespace lord {
class Iconset;
class ResourceLoader;

class LordEnv {
 public:
  static LordEnv* instance();
  static bool InitEnv(int argc, char* argv[]);

  void SetFileSystem(scoped_ptr<azer::FileSystem> filesystem);
  azer::FileSystem* file_system() { return file_system_.get();}
  ResourceLoader* resource_loader() { return resource_loader_.get();}
  nelf::Context* GetNelfContext() { return nelf_context_.get();}
  bool LoadResourcePack(const base::FilePath& path);
  nelf::ResourceBundle* resource_bundle() { return resource_bundle_.get();}
  gfx::ImageSkia GetIcon(int32 id);

  azer::EffectAdapterContext* GetEffectAdapterContext() { return &effect_context_;}
  const DirLight& GetInternalLight() const { return internal_light_;}
  azer::BlendingPtr GetDefaultBlending() { return blending_;}

  azer::Effect* GetEffect(const std::string& name);
 private:
  LordEnv();
  ~LordEnv();

  bool Init(int argc, char* argv[]);
  void InitEffects();
  void InitAdapterContext();

  scoped_ptr<ResourceLoader> resource_loader_;
  scoped_ptr<nelf::ResourceBundle> resource_bundle_;
  scoped_ptr<Iconset> iconset_;
  azer::BlendingPtr blending_;
  DirLight internal_light_;
  nelf::ContextPtr nelf_context_;
  scoped_ptr<azer::FileSystem> file_system_;
  azer::EffectAdapterContext effect_context_;
  std::map<std::string, azer::EffectPtr> effects_;
  static LordEnv* instance_;
  DISALLOW_COPY_AND_ASSIGN(LordEnv);
};
}  // namespace lord
