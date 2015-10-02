#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"

namespace lord {
class RotateControllerObject {
 public:
  RotateControllerObject();
  ~RotateControllerObject();

  void SetSelectedColor(const azer::Vector4& color);

  void Render(const azer::Matrix4& world, 
              const azer::Matrix4& pvw, 
              azer::Renderer* renderer);
 private:
  azer::BlendingPtr blending_;
  GeometryObjectPtr sphere_;
  azer::Vector4 selected_color_;
  azer::Vector4 sphere_color_;
  DISALLOW_COPY_AND_ASSIGN(RotateControllerObject);
};
}  // namepsace lord
