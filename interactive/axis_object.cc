#include "lordaeron/interactive/axis_object.h"

#include "azer/render/render.h"
#include "azer/render/geometry.h"
#include "lordaeron/env.h"
#include "lordaeron/interactive/controller_object.h"
#include "lordaeron/util/picking.h"

namespace lord {
using namespace azer;

// class XYZAxisObject
XYZAxisObject::XYZAxisObject(DiffuseEffect* effect) 
    : effect_(effect),
      length_(1.0f) {
  GeoAxisParams params;
  params.axis_length = 0.82f;
  params.cone_radius = 0.18f;
  params.cone_height = 1.0f - params.axis_length;
  axis_ = CreateLineAxisMeshPart(effect->vertex_desc(), params);
  axis_->SetEffect(effect);
  SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  rotation_[0] = std::move(RotateZ(Degree(90.0f)));
  rotation_[1] = Matrix4::kIdentity;
  rotation_[2] = std::move(RotateX(Degree(-90.0f)));
  reset_color();

  depth_state_ = RenderSystem::Current()->CreateDepthStencilState();
  depth_state_->EnableDepthTest(false);
}

void XYZAxisObject::set_color(const azer::Vector4& col, int32 index) {
  color_[index] = col;
}

void XYZAxisObject::set_length(float length) {
  length_ = length;
}

void XYZAxisObject::reset_color() {
  color_[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
  color_[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
  color_[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
}

void XYZAxisObject::SetPosition(const azer::Vector3& pos) {
  position_ = pos;
}

void XYZAxisObject::Render(const Matrix4& pv, Renderer* renderer) {
  ScopedDepthStencilState scoped_depth_state(renderer, depth_state_);
  LordEnv* context = LordEnv::instance();
  Matrix4 world = std::move(Translate(position_));
  for (int32 i = 0; i < 3; ++i) {
    Matrix4 lworld = std::move(world * rotation_[i] * Scale(1.0f, length_, 1.0f));
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetColor(color_[i]);
    effect_->SetPV(pv);
    effect_->SetWorld(lworld);
    // renderer->UseEffect(effect_.get());
    axis_->Render(renderer);
  }
}

int32 XYZAxisObject::Picking(const azer::Ray& ray, const azer::Camera& camera) {
  const Vector3& pos = position_;
  Plane pxy(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
  Plane pyz(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
  Plane pzx(Vector3(0.0f, 1.0f, 0.0f), -pos.y);

  const float kMargin = 0.05f;
  
  float depth1, depth2, depth3;
  bool parallel1, parallel2, parallel3;
  Vector3 pt1, pt2, pt3;
  PickingPlane(ray, pxy, &pt1, camera, &depth1, &parallel1);
  PickingPlane(ray, pyz, &pt2, camera, &depth2, &parallel2);
  PickingPlane(ray, pzx, &pt3, camera, &depth3, &parallel3);
  if (depth1 >= 0.0f && depth1 < depth2 && depth1 < depth3) {
    if (pt1.x - pos.x <= length_ && pt1.x - pos.x > 0.0 &&
        std::abs(pt1.y - pos.y) < kMargin && std::abs(pt1.y - pos.y) < kMargin) {
      return kPickXAxis;
    } else if (pt1.y - pos.y <= length_ && pt1.y - pos.y > 0.0f &&
               std::abs(pt1.x - pos.x) < kMargin
               && std::abs(pt1.x - pos.x) < kMargin) {
      return kPickYAxis;
    }
  }

  // yzplane
  if (depth2 >= 0.0f && depth2 < depth1 && depth2 < depth3) {
    if (pt2.y - pos.y <= length_ && pt2.y - pos.y > 0.0 &&
        std::abs(pt2.z - pos.z) < kMargin && std::abs(pt2.z - pos.z) < kMargin) {
      return kPickYAxis;
    } else if (pt2.z - pos.z <= length_ && pt2.z - pos.z > 0.0f &&
               std::abs(pt2.y - pos.y) < kMargin 
               && std::abs(pt2.y - pos.y) < kMargin) {
      return kPickZAxis;
    }
  }

  if (depth3 >= 0.0f && depth3 < depth1 && depth3 < depth2) {
    if (pt3.z - pos.z <= length_ && pt3.z - pos.z > 0.0 &&
        std::abs(pt3.x - pos.x) < kMargin && std::abs(pt3.x - pos.x) < kMargin) {
      return kPickZAxis;
    } else if (pt3.x - pos.x <= length_ && pt3.x - pos.x > 0.0f &&
               std::abs(pt3.z - pos.z) < kMargin 
               && std::abs(pt3.z - pos.z) < kMargin) {
      return kPickXAxis;
    }
  }

  return kPickNone;
}
}  // namespace lord
