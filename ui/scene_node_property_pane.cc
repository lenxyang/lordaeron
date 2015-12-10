#include "lordaeron/ui/scene_node_property_pane.h"

namespace nelf {
// class SceneNodePropertyContents
const char SceneNodePropertyContents::kViewClassName[] = "nelf::SceneNodePropertyContents";
SceneNodePropertyContents::SceneNodePropertyContents(SceneNode* node) 
    : node_(node) {
  position_control_ = new VectorControl(node->
}

SceneNodePropertyContents::~SceneNodePropertyContents() {}

const char* SceneNodePropertyContents::GetClassName() const {
  return kViewClassName;
}

gfx::Size SceneNodePropertyContents::GetPreferredSize() const {
  return gfx::Size(320, 240);
}

void SceneNodePropertyContents::Layout() {
  gfx::Rect color_pane_bounds = GetContentsBounds();
  pane_->SetPosition(color_pane_bounds.origin());
}

void SceneNodePropertyContents::OnVectorChanged(VectorControl* control) {
}

// 
const char SceneNodePropertyPane::kViewClassName[] = "nelf::SceneNodePropertyPane";
SceneNodePropertyPane::SceneNodePropertyPane(SceneNode* node)
    : nelf::CollapseView(::base::UTF8ToUTF16("SceneNode")) {
  contents_ = new SceneNodePropertyContents(light);
  GetContents()->AddChildView(contents_);
  GetContents()->SetLayoutManager(new views::FillLayout);
  Expand();
}

const char* SceneNodePropertyPane::GetClassName() const {
  return kViewClassName;
}

void SceneNodePropertyPane::Layout() {
  gfx::Size size = std::move(GetPreferredSize());
  if (parent()) {
    size.set_width(parent()->GetContentsBounds().width());
  }
  SetCollapseSize(size);
  nelf::CollapseView::Layout();
}
}  // namespace nelf
