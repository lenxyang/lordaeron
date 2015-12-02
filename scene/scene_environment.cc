#include "lordaeron/scene/scene_environment.h"

namespace lord {
SceneEnvironment::SceneEnvironment() {
}

SceneEnvironment::~SceneEnvironment() {
}

void SceneEnvironment::reset() {
  light_.clear();
}

void SceneEnvironment::PushLight(LightPtr light) {
  light_.push_back(light);
}

void SceneEnvironment::PopLight() {
  light_.pop_back();
}

void SceneEnvironment::UpdateParams(const azer::FrameArgs& args) {
}
}  // namespace lord
