#pragma once

#include "azer/render/render.h"

namespace lord {

class ColorEffect;
class CoordinateGrid {
 public:
  CoordinateGrid(float width, float height, int num);
  ~CoordinateGrid();
  
  void SetWorld(const azer::Matrix4& world) { world_ = world;}
  void SetProjView(const azer::Matrix4& pv) { pv_ = pv;}
  void Render(azer::Renderer* rs);
  void SetColor(const azer::Vector4& col) { color_ = col;}
 private:
  scoped_refptr<ColorEffect> effect_;
  azer::MeshPartPtr part_;
  azer::Vector4 color_;
  azer::Matrix4 world_;
  azer::Matrix4 pv_;
  const float kWidth, kHeight;
  const int kNum;
  DISALLOW_COPY_AND_ASSIGN(CoordinateGrid);
};
}  // namespace lord
