#include "lordaeron/resource/vertex_desc_loader.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "azer/render/render.h"

namespace lord {
using namespace azer;
const char VertexDescLoader::kSpecialLoaderName[] = "lord::VertexDescLoader";
VertexDescLoader::VertexDescLoader() {
}

VertexDescLoader::~VertexDescLoader() {
}

const char* VertexDescLoader::GetLoaderName() const {
  return kSpecialLoaderName;
}

Resource VertexDescLoader::Load(const ConfigNode* node, ResourceLoaderContext* ctx) {
  ConfigNodes item = std::move(node->GetTaggedChildren("desc"));
  const int32 kVertexDescCount = static_cast<int32>(item.size());
  VertexDesc::Desc* desc = new VertexDesc::Desc[kVertexDescCount];
  VertexDesc::Desc* cur = desc;
  for (auto iter = item.begin(); iter != item.end(); ++iter) {
    ConfigNode* n = iter->get();
    strncmp(cur->name, n->GetAttr("semantic").c_str(), 64);
    if (::base::StringToInt(n->GetAttr("semindex"), &cur->semantic_index)) {
      return Resource();
    }

    if (::base::StringToInt(n->GetAttr("slot"), &cur->input_slot)) {
      return Resource();
    }

    std::string type = n->GetAttr("data_type");
    if (type == "float") {
      cur->type = kFloat;
    } else if (type == "vector2") {
      cur->type = kVec2;
    } else if (type == "vector3") {
      cur->type = kVec3;
    } else if (type == "vector4") {
      cur->type = kVec4;
    } else {
      CHECK(false) << "unsupport type \"" << type << "\"";
    }
    cur++;
  }

  Resource resource;
  resource.type = kResTypeVertexDesc;
  resource.vertex_desc = new VertexDesc(desc, kVertexDescCount);
  resource.retcode = 0;
  return resource;
}

bool VertexDescLoader::CouldLoad(ConfigNode* node) const {
  return node->tagname() == "vertex_desc";
}


VertexDescPtr LoadVertexDesc(const ResPath& path, ResourceLoaderContext* ctx) {
  Resource ret = ctx->loader->Load(path);
  if (ret.retcode != 0) {
    LOG(ERROR) << "Load VertexDesc failed for path: " << path.fullpath();
    return VertexDescPtr();
  }
  if (ret.type != kResTypeVertexDesc) {
    LOG(ERROR) << "Not VertexDesc for path: " << path.fullpath();
    return VertexDescPtr();
  }

  return ret.vertex_desc;
}
}  // namespace lord
