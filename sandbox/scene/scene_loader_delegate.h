#pragma once

#include "base/strings/utf_string_conversions.h"
#include "lordaeron/context.h"
#include "lordaeron/util/model_loader.h"
#include "lordaeron/scene/scene_context.h"
#include "lordaeron/scene/scene_render.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"
#include "lordaeron/scene/scene_loader.h"

namespace lord {
void InitMeshEffect(azer::Effect* effect, azer::Mesh* mesh) {
  for (int32 i = 0; i < mesh->part_count(); ++i) {
    mesh->part_at(i)->SetEffect(effect);
  }
}

class SimpleSceneLoaderDelegate : public SceneLoaderDelegate {
 public:
  SimpleSceneLoaderDelegate(azer::FileSystem* fs, azer::Effect* effect)
      : fsystem_(fs), effect_(effect) {}
  bool InitSceneNode(SceneNode* node, azer::ConfigNode* config) override {
    using azer::ConfigNode;
    Context* ctx = Context::instance(); 
    const std::string& type =  config->GetAttr("type");
    if (type == "mesh") {
      DCHECK(config->HasNamedChild("mesh"));
      ConfigNode* mesh_node = config->GetNamedChildren("mesh")[0];
      DCHECK(mesh_node->HasNamedChild("provider"));
      ConfigNode* provider_node = mesh_node->GetNamedChildren("provider")[0];
      azer::MeshPtr mesh = LoadMesh(mesh_node);
      mesh->SetEffectAdapterContext(ctx->GetEffectAdapterContext());
      mesh->AddProvider(LoadProvider(provider_node));
      node->mutable_data()->AttachMesh(mesh);
      return true;
    } else {
      return true;
    }
  }
  
  azer::MeshPtr LoadMesh(azer::ConfigNode* config) {
    std::string pathstr;
    if (!config->GetChildText("path", &pathstr)) {
      return azer::MeshPtr();
    }
      
    ModelLoader loader(fsystem_);
    azer::MeshPtr obj = loader.Load(azer::ResPath(::base::UTF8ToUTF16(pathstr)), 
                                      effect_->GetVertexDesc());
    InitMeshEffect(effect_, obj.get());
    return obj;
  }
  
  azer::EffectParamsProviderPtr LoadProvider(azer::ConfigNode* config) {
    DiffuseEffectProvider* p(new DiffuseEffectProvider);
    p->SetColor(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
    return azer::EffectParamsProviderPtr(p);
  }
 private:
  azer::FileSystem* fsystem_;
  azer::Effect* effect_;
};

}  // namespace lord
