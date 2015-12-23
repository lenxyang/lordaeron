#pragma once

#include "azer/base/config_node.h"
#include "lordaeron/resource/resource_loader.h"

namespace lord {
using azer::ConfigNode;

ConfigNode* GetTypedReferNode(const std::string& type, const ConfigNode* parent);


azer::VertexDescPtr LoadReferVertexDesc(const ConfigNode* node, 
                                        ResourceLoaderContext* ctx);
azer::EffectPtr LoadReferEffect(const ConfigNode* node,
                                ResourceLoaderContext* ctx);
LightPtr LoadReferLight(const ConfigNode* node, ResourceLoaderContext* ctx);
azer::MeshPtr LoadReferMesh(const ConfigNode* node, ResourceLoaderContext* ctx);
MaterialPtr LoadReferMaterial(const ConfigNode* node, ResourceLoaderContext* ctx);
Resource LoadResource(const azer::ResPath& path, int type,
                      ResourceLoaderContext* ctx);
Resource LoadReferResource(const ConfigNode* node, ResourceLoaderContext* ctx);

bool Repath(const azer::ResPath& path, azer::ResPath* apath,
            ResourceLoaderContext* ctx);
void InitDefaultLoader(ResourceLoader* loder);

}  // namespace lord
