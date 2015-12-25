#include "lordaeron/resource/mesh_loader.h"

#include "base/strings/utf_string_conversions.h"
#include "azer/render/render.h"
#include "lordaeron/resource/mesh_load_util.h"
#include "lordaeron/resource/resource_util.h"

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
VariantResource MeshLoader::Load(const ConfigNode* node, ResourceLoadContext* ctx) {
  ConfigNode* vertex_desc_node = GetTypedReferNode("vertex_desc", node);
  VertexDescPtr vertex_desc = LoadReferVertexDesc(vertex_desc_node, ctx);
  ConfigNode* effect_node = GetTypedReferNode("effect", node);
  EffectPtr effect = LoadReferEffect(effect_node, ctx);
  ConfigNode* material_node = GetTypedReferNode("material", node);
  MaterialPtr material;
  if (material_node) {
    material = LoadReferMaterial(material_node, ctx);
  }

  if (!vertex_desc.get() || !effect.get()) {
    return VariantResource();
  }

  VariantResource resource;
  resource.type = kResTypeMesh;
  resource.mesh = LoadMeshData(node, vertex_desc.get(), ctx);
  resource.retcode = (resource.mesh.get() != NULL) ? 0 : -1;
  if (material.get())
    resource.mesh->AddProvider(material);
  InitMeshEffect(effect.get(), resource.mesh.get());
  return resource;
}

MeshPtr MeshLoader::LoadMeshData(const ConfigNode* node, 
                                 VertexDesc* desc,
                                 ResourceLoadContext* ctx) {
  const ConfigNode* mesh_node = node->GetFirstChildTagged("data");
  if (!mesh_node || !mesh_node->HasAttr("path")) {
    LOG(ERROR) << "model[" << node->GetNodePath() << "] has no effect";
    return MeshPtr();
  }

  ResPath mesh_path(::base::UTF8ToUTF16(mesh_node->GetAttr("path")));
  MeshLoadUtil loader(ctx->filesystem);
  return loader.Load(mesh_path, desc);
}

bool MeshLoader::CouldLoad(ConfigNode* node) const {
  return node->tagname() == "mesh";
}

}  // namespace lord
