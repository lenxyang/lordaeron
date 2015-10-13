#include "lordaeron/effect/scene_node_params.h"

#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
SceneNodeParams::SceneNodeParams(SceneNode* node)
    : node_(node) {
}

SceneNodeParams::~SceneNodeParams() {
}

void SceneNodeParams::UpdateParams(const azer::FrameArgs& args) {
  world_ = node_->world();
}
}
