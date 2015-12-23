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

VariantResource VertexDescLoader::Load(const ConfigNode* node,
                                       ResourceLoadContext* ctx) {
  ConfigNodes item = std::move(node->GetTaggedChildren("desc"));
  const int32 kVertexDescCount = static_cast<int32>(item.size());
  VertexDesc::Desc* desc = new VertexDesc::Desc[kVertexDescCount];
  memset(desc, 0, sizeof(VertexDesc::Desc) * kVertexDescCount);
  VertexDesc::Desc* cur = desc;
  for (auto iter = item.begin(); iter != item.end(); ++iter) {
    ConfigNode* n = iter->get();
    std::string name = n->GetAttr("semantic");
    strncpy(cur->name, name.c_str(), 64);
    if (!::base::StringToInt(n->GetAttr("semindex"), &cur->semantic_index)) {
      LOG(ERROR) << "Invalid semindex: \"" << n->GetAttr("semindex");
      return VariantResource();
    }

    if (!::base::StringToInt(n->GetAttr("slot"), &cur->input_slot)) {
      LOG(ERROR) << "Invalid semindex: \"" << n->GetAttr("slot");
      return VariantResource();
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

  VariantResource resource;
  resource.type = kResTypeVertexDesc;
  resource.vertex_desc = new VertexDesc(desc, kVertexDescCount);
  resource.retcode = 0;
  return resource;
}

bool VertexDescLoader::CouldLoad(ConfigNode* node) const {
  return node->tagname() == "vertex_desc";
}

}  // namespace lord
