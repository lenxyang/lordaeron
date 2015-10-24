#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"
#include "ui/gfx/image/image_skia.h"
#include "nelf/nelf.h"

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

  azer::EffectAdapterContext* GetEffectAdapterContext() { return &context_;}
  const azer::DirLight& GetInternalLight() const { return internal_light_;}
  azer::BlendingPtr GetDefaultBlending() { return blending_;}
 private:
  Context();
  ~Context();

  bool Init(int argc, char* argv[]);
  void InitAdapterContext();

  scoped_ptr<nelf::ResourceBundle> resource_bundle_;
  scoped_ptr<Iconset> iconset_;
  azer::BlendingPtr blending_;
  azer::DirLight internal_light_;
  nelf::ContextPtr nelf_context_;
  azer::EffectAdapterContext context_;
  static Context* instance_;
  DISALLOW_COPY_AND_ASSIGN(Context);
};
}  // namespace lord
