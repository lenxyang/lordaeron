#include "lordaeron/resource/mesh_loader.h"

#include "base/strings/utf_string_conversions.h"
#include "azer/render/render.h"
#include "lordaeron/resource/effect_loader.h"
#include "lordaeron/resource/mesh_load_util.h"
#include "lordaeron/resource/vertex_desc_loader.h"

namespace lord {
using namespace azer;
namespace {
void InitMeshEffect(Effect* effect, Mesh* mesh) {
  for (int32 i = 0; i < mesh->part_count(); ++i) {
    mesh->part_at(i)->SetEffect(effect);
  }
}
}  // namespace

const char MeshLoader::kSpecialLoaderName[] = "lord::MeshLoader";
MeshLoader::MeshLoader() {
}

MeshLoader::~MeshLoader() {
}

const char* MeshLoader::GetLoaderName() const {
  return kSpecialLoaderName;
}
Resource MeshLoader::Load(const ConfigNode* node, ResourceLoaderContext* ctx) {
  ResPath vertex_desc_path(::base::UTF8ToUTF16(node->GetAttr("vertex_desc")));
  VertexDescPtr vertex_desc = LoadVertexDesc(vertex_desc_path, ctx);
  if (!vertex_desc.get()) {
    return Resource();
  }

  EffectPtr effect = this->LoadEffect(node, ctx);
  if (!effect.get()) {
    return Resource();
  }

  Resource resource;
  resource.type = kResTypeMesh;
  resource.mesh = LoadMeshData(node, vertex_desc.get(), ctx);
  resource.retcode = (resource.mesh.get() != NULL) ? 0 : -1;
  InitMeshEffect(effect.get(), resource.mesh.get());
  return resource;
}

MeshPtr MeshLoader::LoadMeshData(const ConfigNode* node, 
                                 VertexDesc* desc,
                                 ResourceLoaderContext* ctx) {
  const ConfigNode* mesh_node = node->GetFirstChildTagged("model");
  if (!mesh_node || !mesh_node->HasAttr("path")) {
    LOG(ERROR) << "model[" << node->GetNodePath() << "] has no effect";
    return MeshPtr();
  }

  ResPath mesh_path(::base::UTF8ToUTF16(mesh_node->GetAttr("path")));
  MeshLoadUtil loader(ctx->filesystem);
  return loader.Load(mesh_path, desc);
}

EffectPtr MeshLoader::LoadEffect(const ConfigNode* node, 
                                 ResourceLoaderContext* ctx) {
  const ConfigNode* effect_node = node->GetFirstChildTagged("effect");
  if (!effect_node || !effect_node->HasAttr("refpath")) {
    LOG(ERROR) << "effect[" << node->GetNodePath() << "] has no effect";
    return EffectPtr();
  }
  ResPath effect_path(::base::UTF8ToUTF16(effect_node->GetAttr("refpath")));
  EffectPtr effect = lord::LoadEffect(effect_path, ctx);
  if (!effect.get()) {
    return EffectPtr();
  }

  return effect;
}

bool MeshLoader::CouldLoad(ConfigNode* node) const {
  return node->tagname() == "mesh";
}

}  // namespace lord
