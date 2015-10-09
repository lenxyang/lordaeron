#include "lordaeron/render/mesh.h"

namespace lord {

using namespace azer;
Mesh::Mesh() {
}

Mesh::~Mesh() {
}

void Mesh::Update(const FrameArgs& args) {
}

void Mesh::Draw(Renderer* renderer, PrimitiveTopology primitive) {
  for (int32 i = 0; i < static_cast<int32>(entity_.size()); ++i) {
    ApplyEffectParams(i, renderer);
    Entity& entity = entity_[i];
    renderer->Draw(entity.vb.get(), primitive);
  }
}

void Mesh::ApplyEffectParams(int32 index, Renderer* renderer) {
  Group& group = group_[index];
  EffectPtr effect = group.effect;
  DCHECK(group.provider.size() == group.adapter.size());
  for (int32 i = 0; i < static_cast<int32>(group.provider.size()); ++i) {
    EffectParamsProviderPtr provider = provider_[group.provider[i]];
    EffectParamsAdapter* adapter = adapter_[group.adapter[i]];
    adapter->Apply(effect.get(), provider);
  }
  effect->Use(renderer);
}

void Mesh::DrawIndex(Renderer* renderer, PrimitiveTopology primitive) {
  for (int32 i = 0; i < static_cast<int32>(entity_.size()); ++i) {
    ApplyEffectParams(i, renderer);
    Entity& entity = entity_[i];
    renderer->DrawIndex(entity.vb.get(), entity.ib.get(), primitive);
  }
}
}  // namespace lord
