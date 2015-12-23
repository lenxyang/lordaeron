#include "lordaeron/resource/effect_loader.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "azer/render/render.h"
#include "azer/render/effect_creator.h"

namespace lord {
using namespace azer;
using base::UTF8ToUTF16;
using base::UTF16ToUTF8;

namespace {
bool LoadFile(const ResPath& path, FileContents* contents, FileSystem* fs) {
  FilePtr file = fs->OpenFile(path);
  if (!file.get()) {
    LOG(ERROR) << "Failed to open file: " << path.fullpath();
    return false;
  }

  if (!file->PRead(0, -1, contents)) {
    LOG(ERROR) << "Failed to read file: " << path.fullpath();
    return false;
  }
  return true;
}
}

const char EffectLoader::kSpecialLoaderName[] = "lord::EffectLoader";
EffectLoader::EffectLoader() {
}

EffectLoader::~EffectLoader() {
}

const char* EffectLoader::GetLoaderName() const {
  return kSpecialLoaderName;
}

namespace {
int GetStageFromName(const std::string& name) {
  if (name == "vertex") {
    return kVertexStage;
  } else if (name == "pixel") {
    return kPixelStage;
  } else if (name == "geometry") {
    return kGeometryStage;
  } else {
    CHECK(false) << "no supported stage \"" << name << "\"";
    return -1;
  }
}
}

Resource EffectLoader::Load(const azer::ConfigNode* node, 
                            ResourceLoaderContext* ctx) {
  Effect::ShaderPrograms program;
  program.resize(kRenderPipelineStageNum);
  ConfigNodes item = node->GetTaggedChildren("shader");
  FileContents contents;
  for (auto iter = item.begin(); iter != item.end(); ++iter) {
    contents.clear();
    ConfigNode* n = iter->get();
    GpuProgram::ShaderInfo info;
    info.stage = GetStageFromName(n->GetAttr("stage"));
    info.entry = n->GetAttr("entry");
    info.version = n->GetAttr("version");
    info.path = n->GetAttr("path");
    ResPath respath(UTF8ToUTF16(info.path));
    if (!LoadFile(respath, &contents, ctx->filesystem)) {
      LOG(ERROR) << "Failed to loader path: " << info.path;
      return Resource();
    }
    info.code = std::string((const char*)&contents.front(), contents.size());
    program[info.stage] = info;
  }

  azer::EffectPtr effect = CreateEffectByName(node->GetAttr("effect_name"));
  if (!effect.get() || !effect->Init(program)) {
    LOG(ERROR) << "Failed to init effect \"" << node->GetAttr("effect_name") << "\"";
    return Resource();
  }

  Resource resource;
  resource.type = kResTypeEffect;
  resource.effect = effect;
  resource.retcode = 0;
  return resource;
}

bool EffectLoader::CouldLoad(azer::ConfigNode* node) const {
  return node->tagname() == "effect";
}

}  // namespace lord
