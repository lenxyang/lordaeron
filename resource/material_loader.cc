#include "lordaeron/resource/material_loader.h"

#include "base/strings/utf_string_conversions.h"
#include "azer/render/render.h"
#include "azer/render/effect_creator.h"
#include "lordaeron/effect/material.h"

namespace lord {
using namespace azer;

const char MaterialLoader::kSpecialLoaderName[] = "lord::MaterialLoader";
MaterialLoader::MaterialLoader() {
}

MaterialLoader::~MaterialLoader() {
}

const char* MaterialLoader::GetLoaderName() const {
  return kSpecialLoaderName;
}

VariantResource MaterialLoader::Load(const ConfigNode* node,
                                     ResourceLoadContext* ctx) {
  EffectParamsProviderPtr provider = 
      CreateEffectProviderByName(node->GetAttr("provider_name"));
  Material* mtrl = dynamic_cast<Material*>(provider.get());
  if (!mtrl || !mtrl->Init(node)) {
    return VariantResource();
  }

  VariantResource ret;
  ret.type = kResTypeMaterial;
  ret.retcode = 0;
  ret.material = mtrl;
  return ret;
}

bool MaterialLoader::CouldLoad(ConfigNode* node) const {
  return node->tagname() == "material";
}
}  // namespace lord
