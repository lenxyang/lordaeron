#include "lordaeron/render/coordinate.h"

#include "azer/render/geometry.h"
#include "lordaeron/effect/color_effect.h"

namespace lord {
using namespace azer;
CoordinateGrid::CoordinateGrid(float width, float height, int num) 
    : color_(Vector4(1.0f, 0.0f, 0.0f, 1.0)),
      kWidth(width), kHeight(height), kNum(num) {
  world_ = Matrix4::kIdentity;
  effect_ = CreateColorEffect();
  float total_width = kWidth * kNum;
  float total_height = kHeight * kNum;
  const int32 kCount = (kNum + 1) * 4;
  scoped_ptr<Vector3> ptr(new Vector3[kCount]);
  Vector3* cur = ptr.get();
  for (int i = 0; i < kNum + 1; ++i) {
    float z = total_height / 2.0f - kHeight * i;
    *cur++ = Vector3(-total_width / 2.0f, 0.0f, z);
    *cur++ = Vector3( total_width / 2.0f, 0.0f, z);
  }

  for (int i = 0; i < kNum + 1; ++i) {
    float x  = total_width / 2.0f - kWidth * i;
    *cur++ = Vector3(x, 0.0f, -total_height / 2.0f);
    *cur++ = Vector3(x, 0.0f,  total_height / 2.0f);
  }

  EntityPtr entity = CreateGeoPointsList(ptr.get(), kCount, effect_->vertex_desc());
  entity->set_topology(kLineList);
  part_ = new MeshPart(effect_);
  part_->AddEntity(entity);
}

CoordinateGrid::~CoordinateGrid() {
}

void CoordinateGrid::Render(azer::Renderer* rs) {
  effect_->SetPVW(pv_ * world_);
  effect_->SetColor(color_);
  part_->Render(rs);
}
}  // namespace lord
