#include "lordaeron/scene/scene_mesh.h"

#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
SceneMesh::SceneMesh(SceneNode* node)
    : node_(node) {
}

SceneMesh::~SceneMesh() {
}
}  // namespace lord
