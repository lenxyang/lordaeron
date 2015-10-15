#include "lordaeron/effect/global_environemnt_params.h"

namespace lord {
GlobalEnvironmentParams::GlobalEnvironmentParams() 
    : camera_(NULL) {
  projview_ = azer::Matrix4::kIdentity;
}

GlobalEnvironmentParams::~GlobalEnvironmentParams() {
}

void GlobalEnvironmentParams::UpdateParams(const azer::FrameArgs& args) {
  if (camera_) {
    projview_ = camera_->GetProjViewMatrix();
  }
}
}  // namespace lord
