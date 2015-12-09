#include "lordaeron/ui/scene_node_property_pane.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/views/layout/fill_layout.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/light_pane.h"

namespace lord {
using base::UTF8ToUTF16;
const char SceneNodePropertyPane::kViewClassName[] = "nelf::SceneNodePropertyPane";
SceneNodePropertyPane::SceneNodePropertyPane() 
    : node_(NULL) {
  ClearUI();
}

const char* SceneNodePropertyPane::GetClassName() const {
  return kViewClassName;
}

void SceneNodePropertyPane::ClearUI() {
  RemoveAllChildViews(true);
  container_ = new nelf::CollapseViewContainer;
  AddChildView(container_->CreateParentIfNecessary());
  SetLayoutManager(new views::FillLayout);
}

void SceneNodePropertyPane::SetSceneNode(SceneNode* node) {
  ClearUI();
  node_ = node;
  if (node_) {
    InitUIForCommonNode();
    switch (node->type()) {
      case kLampSceneNode:
        InitUIForLampNode();
        break;
      case kObjectSceneNode:
        InitUIForObjectNode();
        break;
      default:
        break;
    }
  }
}

void SceneNodePropertyPane::Layout() {
  views::View* child = child_at(0);
  if (child) {
    child->SetBoundsRect(std::move(GetContentsBounds()));
    gfx::Size size = container_->GetPreferredSize();
    size.set_width(child->GetContentsBounds().width());
    container_->SetBounds(0, 0, size.width(), size.height());
  }
}

void SceneNodePropertyPane::InitUIForLampNode() {
  Light* light = node_->mutable_data()->light();
  switch (light->type()) {
    case kDirectionalLight: {
      DirectionalLightPane* pane = new DirectionalLightPane(light);
      container_->AddChildView(pane);
      break;
    }
    default:
      break;
  }
}

void SceneNodePropertyPane::InitUIForObjectNode() {
}

void SceneNodePropertyPane::InitUIForCommonNode() {
}

void SceneNodePropertyPane::OnSceneNodeSelected(InteractiveContext* context, 
                                                SceneNode* prevsel) {
  SetSceneNode(context->GetPickingNode());
  Layout();
}

// class SceneNodePropertyWindow
const char SceneNodePropertyWindow::kViewClassName[] = "nelf::SceneNodePropertyWindow";
SceneNodePropertyWindow::SceneNodePropertyWindow(const gfx::Rect& bounds, 
                                                 nelf::MainFrame* mainframe)
    : nelf::TabbedWindow(bounds, mainframe),
      property_pane_(NULL) {
  nelf::Pane* pane = new nelf::Pane();
  property_pane_ = new SceneNodePropertyPane;
  pane->GetContents()->AddChildView(property_pane_);
  pane->GetContents()->SetLayoutManager(new views::FillLayout);
  pane->SetTitle(::base::UTF8ToUTF16("Property"));
  AddPane(pane);
  Layout();
}

void SceneNodePropertyWindow::SetSceneNode(SceneNode* node) {
  property_pane_->SetSceneNode(node);
}

const char* SceneNodePropertyWindow::GetClassName() const {
  return kViewClassName;
}
}  // namespace lord
