#include "lordaeron/scene/scene_context.h"

#include "lordaeron/scene/scene_node_data.h"
#include "lordaeron/scene/scene_environment.h"
#include "lordaeron/effect/global_environemnt_params.h"

namespace lord {
SceneContext::SceneContext() {
  node_environment_.reset(new SceneEnvironment);
  global_environment_ = new GlobalEnvironmentParams;
}

SceneContext::~SceneContext() {
}

SceneEnvironment* SceneContext::GetNodeEnvironment() { 
  return node_environment_.get();
}

GlobalEnvironmentParams* SceneContext::GetGlobalEnvironment() { 
  return global_environment_.get();
}

}  // namespace lord
