#include "lordaeron/resource/resource_util.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "azer/base/file_system.h"
#include "lordaeron/util/xml_util.h"
#include "lordaeron/resource/effect_loader.h"
#include "lordaeron/resource/light_loader.h"
#include "lordaeron/resource/material_loader.h"
#include "lordaeron/resource/mesh_loader.h"
#include "lordaeron/resource/scene_loader.h"
#include "lordaeron/resource/vertex_desc_loader.h"

namespace lord {
using namespace azer;
bool Repath(const ResPath& path, ResPath* apath, ResourceLoadContext* ctx) {
  CHECK(!path.empty());
  CHECK(!ctx->path.empty());
  if (path.IsAbsolutePath()) {
    *apath = path;
  } else if (!path.component().empty() && path.filepath().empty()) {
    *apath = ResPath(ctx->path.filepath().as_string());
    CHECK(apath->Append(ResPath(path.component().as_string())));
  } else {
    *apath = ResPath(ctx->path.filepath().as_string());
    CHECK(apath->Append(ResPath(path.fullpath())));
  }
  return !apath->empty();
}

void InitDefaultLoader(ResourceLoader* loader) {
  loader->RegisterSpecialLoader(new EffectLoader);
  loader->RegisterSpecialLoader(new MeshLoader);
  loader->RegisterSpecialLoader(new VertexDescLoader);
  loader->RegisterSpecialLoader(new LightLoader);
  loader->RegisterSpecialLoader(new SceneLoader);
  loader->RegisterSpecialLoader(new MaterialLoader);
}

MeshPtr LoadReferMesh(const ConfigNode* node, ResourceLoadContext* ctx) {
  VariantResource ret = LoadReferResource(node, ctx);
  if (ret.type == kResTypeMesh) {
    return ret.mesh;
  } else {
    return MeshPtr();
  }
}

LightPtr LoadReferLight(const ConfigNode* node, ResourceLoadContext* ctx) {
  VariantResource ret = LoadReferResource(node, ctx);
  if (ret.type == kResTypeLight) {
    return ret.light;
  } else {
    return LightPtr();
  }
}

EffectPtr LoadReferEffect(const ConfigNode* node, ResourceLoadContext* ctx) {
  VariantResource ret = LoadReferResource(node, ctx);
  if (ret.type == kResTypeEffect) {
    return ret.effect;
  } else {
    return EffectPtr();
  }
}

VertexDescPtr LoadReferVertexDesc(const ConfigNode* node, 
                                  ResourceLoadContext* ctx) {
  VariantResource ret = LoadReferResource(node, ctx);
  if (ret.type == kResTypeVertexDesc) {
    return ret.vertex_desc;
  } else {
    return VertexDescPtr();
  }
}

MaterialPtr LoadReferMaterial(const ConfigNode* node, ResourceLoadContext* ctx) {
  VariantResource ret = LoadReferResource(node, ctx);
  if (ret.type == kResTypeMaterial) {
    return ret.material;
  } else {
    return MaterialPtr();
  }
}

VertexDescPtr LoadVertexDesc(const ResPath& path, ResourceLoadContext* ctx) {
  VariantResource res = LoadResource(path, kResTypeVertexDesc, ctx);
  return res.vertex_desc;
}

EffectPtr LoadEffect(const ResPath& path, ResourceLoadContext* ctx) {
  VariantResource res = LoadResource(path, kResTypeEffect, ctx);
  return res.effect;
}

VariantResource LoadResource(const azer::ResPath& path, int type,
                             ResourceLoader* loader) {
  CHECK(path.IsAbsolutePath());
  ResourceLoadContext ctx;
  ctx.path = path;
  ctx.loader = loader;
  ctx.filesystem = loader->file_system();
  return LoadResource(path, type, &ctx);
}

VariantResource LoadResource(const ResPath& path, int type, ResourceLoadContext* ctx) {
  CHECK(!path.empty());
  ResPath npath;
  CHECK(Repath(path, &npath, ctx));
  VariantResource ret = ctx->loader->Load(npath);
  if (ret.retcode != 0) {
    LOG(ERROR) << "Load Effect failed for path: " << npath.fullpath();
    return VariantResource();
  }
  if (ret.type != type) {
    LOG(ERROR) << "Not Effect for path: " << npath.fullpath();
    return VariantResource();
  }

  return ret;
}

int32 GetTypeFromString(const std::string& str) {
  if (str == "mesh") {
    return kResTypeMesh;
  } else if (str == "material") {
    return kResTypeMaterial;
  } else if (str == "effect") {
    return kResTypeEffect;
  } else if (str == "light") {
    return kResTypeLight;
  } else if (str == "scene") {
    return kResTypeScene;
  } else if (str == "vertex_desc") {
    return kResTypeVertexDesc;
  } else {
    NOTREACHED() << "unknown type: " << str;
    return kResTypeNone;
  }
}

VariantResource LoadReferResource(const ConfigNode* node, ResourceLoadContext* ctx) {
  if (!node) {
    return VariantResource();
  }
  DCHECK(node->tagname() == "refer");
  ResPath path(::base::UTF8ToUTF16(node->GetAttr("path")));
  CHECK(!path.empty());
  int type = GetTypeFromString(node->GetAttr("type"));
  return LoadResource(path, type, ctx);
}

ConfigNode* GetTypedReferNode(const std::string& type_name,
                              const ConfigNode* parent) {
  ConfigNodes nodes = parent->GetTaggedChildren("refer");
  for (auto iter = nodes.begin(); iter != nodes.end(); ++iter) {
    if ((*iter)->GetAttr("type") == type_name) {
      return (*iter).get();
    }
  }

  return NULL;
}
}  // namespace lord
