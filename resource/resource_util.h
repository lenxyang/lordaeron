#pragma once

#include "azer/base/config_node.h"
#include "lordaeron/resource/resource_loader.h"

namespace lord {
using azer::ConfigNode;
using azer::ResPath;

ConfigNode* GetTypedReferNode(const std::string& type, const ConfigNode* parent);


azer::VertexDescPtr LoadReferVertexDesc(const ConfigNode* node, 
                                        ResourceLoadContext* ctx);
azer::EffectPtr LoadReferEffect(const ConfigNode* node,
                                ResourceLoadContext* ctx);
LightPtr LoadReferLight(const ConfigNode* node, ResourceLoadContext* ctx);
azer::MeshPtr LoadReferMesh(const ConfigNode* node, ResourceLoadContext* ctx);
MaterialPtr LoadReferMaterial(const ConfigNode* node, ResourceLoadContext* ctx);
VariantResource LoadReferResource(const ConfigNode* node, ResourceLoadContext* ctx);

azer::VertexDescPtr LoadVertexDesc(const ResPath& path, ResourceLoadContext* ctx);
azer::EffectPtr LoadEffect(const ResPath& path, ResourceLoadContext* ctx);

VariantResource LoadResource(const ResPath& path, int type, ResourceLoadContext* ctx);
VariantResource LoadResource(const ResPath& path, int type, ResourceLoader* loader);


bool Repath(const ResPath& path, ResPath* apath,
            ResourceLoadContext* ctx);
void InitDefaultLoader(ResourceLoader* loder);

}  // namespace lord
