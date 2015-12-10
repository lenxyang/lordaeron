#include "lordaeron/ui/scene_node_inspector_pane.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/views/layout/fill_layout.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/light_pane.h"

namespace lord {
using base::UTF8ToUTF16;
const char SceneNodeInspectorPane::kViewClassName[] = "nelf::SceneNodeInspectorPane";
SceneNodeInspectorPane::SceneNodeInspectorPane() 
    : node_(NULL) {
  ClearUI();
}

const char* SceneNodeInspectorPane::GetClassName() const {
  return kViewClassName;
}

void SceneNodeInspectorPane::ClearUI() {
  RemoveAllChildViews(true);
  container_ = new nelf::CollapseViewContainer;
  AddChildView(container_->CreateParentIfNecessary());
  SetLayoutManager(new views::FillLayout);
}

void SceneNodeInspectorPane::SetSceneNode(SceneNode* node) {
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

void SceneNodeInspectorPane::Layout() {
  views::View* child = child_at(0);
  if (child) {
    child->SetBoundsRect(std::move(GetContentsBounds()));
    gfx::Size size = container_->GetPreferredSize();
    size.set_width(child->GetContentsBounds().width());
    container_->SetBounds(0, 0, size.width(), size.height());
  }
}

void SceneNodeInspectorPane::InitUIForLampNode() {
  Light* light = node_->mutable_data()->light();
  switch (light->type()) {
    case kDirectionalLight: {
      DirectionalLightPane* pane = new DirectionalLightPane(light);
      container_->AddChildView(pane);
      break;
    }
    case kSpotLight: {
      SpotLightPane* pane = new SpotLightPane(light);
      container_->AddChildView(pane);
      break;
    }
    case kPointLight: {
      PointLightPane* pane = new PointLightPane(light);
      container_->AddChildView(pane);
      break;
    }
    default:
      break;
  }
}

void SceneNodeInspectorPane::InitUIForObjectNode() {
}

void SceneNodeInspectorPane::InitUIForCommonNode() {
}

void SceneNodeInspectorPane::OnSceneNodeSelected(InteractiveContext* context, 
                                                 SceneNode* prevsel) {
  SetSceneNode(context->GetPickingNode());
  Layout();
}

// class SceneNodeInspectorWindow
const char SceneNodeInspectorWindow::kViewClassName[] = "nelf::SceneNodeInspectorWindow";
SceneNodeInspectorWindow::SceneNodeInspectorWindow(const gfx::Rect& bounds, 
                                                   nelf::MainFrame* mainframe)
    : nelf::TabbedWindow(bounds, mainframe),
      inspector_pane_(NULL) {
  nelf::Pane* pane = new nelf::Pane();
  inspector_pane_ = new SceneNodeInspectorPane;
  pane->GetContents()->AddChildView(inspector_pane_);
  pane->GetContents()->SetLayoutManager(new views::FillLayout);
  pane->SetTitle(::base::UTF8ToUTF16("Inspector"));
  AddPane(pane);
  Layout();
}

void SceneNodeInspectorWindow::SetSceneNode(SceneNode* node) {
  inspector_pane_->SetSceneNode(node);
}

const char* SceneNodeInspectorWindow::GetClassName() const {
  return kViewClassName;
}
}  // namespace lord
