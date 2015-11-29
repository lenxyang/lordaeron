#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {

enum {
  kPickNone,
  kPickXAxis,
  kPickYAxis,
  kPickZAxis,
  kPickXYPlane,
  kPickYZPlane,
  kPickZXPlane,
};

azer::Vector4 CalcWorldPosFromScreen(const gfx::Point& pt, const gfx::Size& size, 
                                     const azer::Camera* camera);

azer::Ray GetPickingRay(const gfx::Point& pt, const gfx::Size& size,
                        const azer::Camera* camera);

class PlanePickingHelper {
 public:
  PlanePickingHelper(const azer::Plane& plane);

  void SetShowPicking(bool show_picking);
  void SetPickingPos(const azer::Vector3& pos);
  void set_plane_color(const azer::Vector4& c) { plane_color_ = c;}
  void set_point_color(const azer::Vector4& c) { point_color_ = c;}
  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);
 private:
  void CreatePlane();
  bool show_picking_;
  azer::Vector4 point_color_;
  azer::Vector4 plane_color_;
  azer::Vector3 position_;
  azer::EntityPtr plane_object_;
  azer::EntityPtr sphere_;
  azer::Plane plane_;
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(PlanePickingHelper);
};

void PickingPlane(const azer::Ray& ray, const azer::Plane& plane, 
                  azer::Vector3* pt, const azer::Camera& camera, float* depth, 
                  bool* parallel);
}  // namespace lord
