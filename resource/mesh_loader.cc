#include "lordaeron/resource/mesh_loader.h"

#include "base/strings/utf_string_conversions.h"
#include "azer/render/render.h"
#include "lordaeron/resource/mesh_load_util.h"
#include "lordaeron/resource/vertex_desc_loader.h"

namespace lord {
using namespace azer;
const char MeshLoader::kSpecialLoaderName[] = "lord::MeshLoader";
MeshLoader::MeshLoader() {
}

MeshLoader::~MeshLoader() {
}

const char* MeshLoader::GetLoaderName() const {
  return kSpecialLoaderName;
}
Resource MeshLoader::Load(const ConfigNode* node, ResourceLoaderContext* ctx) {
  ResPath refpath(::base::UTF8ToUTF16(node->GetAttr("effect")));
  ResPath vertex_desc_path(::base::UTF8ToUTF16(node->GetAttr("vertex_desc")));
  VertexDescPtr vertex_desc = LoadVertexDesc(vertex_desc_path, ctx);
  if (!vertex_desc.get()) {
    return Resource();
  }
  std::string pathstr;
  if (!node->GetChildText("path", &pathstr)) {
    LOG(ERROR) << "node[" << node->GetNodePath() << "] has no path";
    return Resource();
  }

  ResPath meshpath(::base::UTF8ToUTF16(pathstr));
  MeshLoadUtil loader(ctx->filesystem);
  Resource resource;
  resource.type = kResTypeMesh;
  resource.mesh = loader.Load(meshpath, vertex_desc);
  resource.retcode = (resource.mesh.get() != NULL) ? 0 : -1;
  return resource;
}

bool MeshLoader::CouldLoad(ConfigNode* node) const {
  const std::string attr = std::move(node->GetAttr("type"));
  return node->tagname() == "node" && !attr.empty() && attr == "mesh";
}

}  // namespace lord
