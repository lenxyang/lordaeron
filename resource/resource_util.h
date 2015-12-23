#pragma once

#include "azer/base/config_node.h"
#include "lordaeron/resource/resource_loader.h"

namespace lord {
using azer::ConfigNode;

ConfigNode* GetTypedReferNode(const std::string& type, const ConfigNode* parent);


azer::VertexDescPtr LoadReferVertexDesc(const ConfigNode* node, 
                                        ResourceLoadContext* ctx);
azer::EffectPtr LoadReferEffect(const ConfigNode* node,
                                ResourceLoadContext* ctx);
LightPtr LoadReferLight(const ConfigNode* node, ResourceLoadContext* ctx);
azer::MeshPtr LoadReferMesh(const ConfigNode* node, ResourceLoadContext* ctx);
MaterialPtr LoadReferMaterial(const ConfigNode* node, ResourceLoadContext* ctx);
VariantResource LoadReferResource(const ConfigNode* node, ResourceLoadContext* ctx);
VariantResource LoadResource(const azer::ResPath& path, int type,
                      ResourceLoadContext* ctx);

bool Repath(const azer::ResPath& path, azer::ResPath* apath,
            ResourceLoadContext* ctx);
void InitDefaultLoader(ResourceLoader* loder);

}  // namespace lord
