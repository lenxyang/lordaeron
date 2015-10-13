#pragma once

#include <deque>
#include <vector>

#include "lordaeron/render/light.h"

namespace lord {
class SceneEnvironment {
 public:
  SceneEnvironment();
  ~SceneEnvironment();

  void reset();

  void PushLight(LightPtr light);
  void PopLight();

  const std::deque<LightPtr>& light() const { return light_;}
 private:
  std::deque<LightPtr> light_;
  DISALLOW_COPY_AND_ASSIGN(SceneEnvironment);
};
}  // namespace lord
