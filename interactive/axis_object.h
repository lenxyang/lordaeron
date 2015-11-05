#pragma once

#include "azer/render/render.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
class AxisObject {
 public:
  AxisObject(azer::VertexDesc* desc);
  ~AxisObject();

  void set_length(float length);
  float length() const { return length_;}
  void Render(azer::Renderer* renderer);
 private:
  void CreateCone(azer::VertexDesc* desc);
  void CreateLine(azer::VertexDesc* desc);
  float length_;
  float cone_radius_;
  float cone_height_;
  azer::EntityPtr cone_;
  azer::EntityPtr line_;
  azer::Matrix4 rotation_[3];
  azer::Vector3 position_;
  azer::VertexDescPtr desc_;
  DISALLOW_COPY_AND_ASSIGN(AxisObject);
};

class XYZAxisObject {
 public:
  XYZAxisObject(DiffuseEffect* effect);
  void set_length(float length);

  const azer::Vector3& position() const { return position_;}
  void SetPosition(const azer::Vector3& pos);
  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);

  void set_color(const azer::Vector4& col, int32 index);
  void reset_color();

  int32 Picking(const azer::Ray& ray);
 private:
  azer::Vector4 color_[3];
  azer::Matrix4 rotation_[3];
  azer::Vector3 position_;
  DiffuseEffectPtr effect_;
  scoped_ptr<AxisObject> object_;
  DISALLOW_COPY_AND_ASSIGN(XYZAxisObject);
};
}  // namespace lord
