#pragma once

#include <vector>
#include "base/memory/ref_counted.h"
#include "azer/render/render.h"
#include "lordaeron/render/mesh.h"
#include "lordaeron/render/entity_data.h"

namespace lord {

class Mesh : public ::base::RefCounted<Mesh> {
 public:
  struct Entity {
    azer::VertexBufferPtr vb;
    azer::IndicesBufferPtr ib;
    int32 group_index;
  };

  struct Group {
    azer::EffectPtr effect;
    std::vector<int32> provider;
    std::vector<int32> adapter;
  };

  Mesh();
  ~Mesh();

  void AddEntityData(EntityData* data);
  void AddEntityBuffer(EntityBuffer* data);

  void Update(const azer::FrameArgs& args);
  void Draw(azer::Renderer* renderer, azer::PrimitiveTopology primitive);
  void DrawIndex(azer::Renderer* renderer, azer::PrimitiveTopology primitive);
 private:
  void ApplyEffectParams(int32 group_index, azer::Renderer* renderer);
  std::vector<Entity> entity_;
  std::vector<Group> group_;
  std::vector<azer::EffectParamsProviderPtr> provider_;
  std::vector<azer::EffectParamsAdapter*> adapter_;
  DISALLOW_COPY_AND_ASSIGN(Mesh);
};
}  // namespace lord
