#include "lordaeron/render/coordinate.h"

#include "azer/render/geometry.h"
#include "lordaeron/env.h"
#include "lordaeron/effect/diffuse_effect.h"
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

namespace {
void MergeMeshPart(MeshPart* merge_to, MeshPart* part) {
  for (int32 i = 0; i < part->entity_count(); ++i) {
    merge_to->AddEntity(part->entity_at(i));
  }
}
}

AxesFrame::AxesFrame() {
  LordEnv* env = LordEnv::instance();
  effect_ = (DiffuseEffect*)env->GetEffect(DiffuseEffect::kEffectName);

  world_ = Matrix4::kIdentity;
  GeoSphereParams sphere_params;
  GeoAxisParams axis_params;
  sphere_params.slice = sphere_params.stack = 64;
  sphere_params.radius = 0.1f;
  sphere_ = CreateSphereMeshPart(effect_->vertex_desc(), sphere_params);
  sphere_->SetEffect(effect_);
  axis_params.cone_radius = 0.06f;
  axis_params.cone_height = 0.2f;
  axis_params.axis_length = 0.8f;
  axis_params.axis_radius = 0.04f;
  axis_params.slice = 64;

  Matrix4 matx = RotateZ(Degree(90.0f));
  Matrix4 matz = RotateX(Degree(-90.0f));
  axisx_ = CreateAxisMeshPart(effect_->vertex_desc(), matx, axis_params);
  axisy_ = CreateAxisMeshPart(effect_->vertex_desc(), axis_params);
  axisz_ = CreateAxisMeshPart(effect_->vertex_desc(), matz, axis_params);
  axisx_->SetEffect(effect_);
  axisy_->SetEffect(effect_);
  axisz_->SetEffect(effect_);
}

AxesFrame::~AxesFrame() {}

void AxesFrame::Render(azer::Renderer* rs) {
  LordEnv* env = LordEnv::instance();
  effect_->SetPV(pv_);
  effect_->SetWorld(world_);
  effect_->SetColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
  effect_->SetDirLight(env->GetInternalLight());
  sphere_->Render(rs);

  effect_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  axisx_->Render(rs);
  effect_->SetColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
  axisy_->Render(rs);
  effect_->SetColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  axisz_->Render(rs);
}
}  // namespace lord
