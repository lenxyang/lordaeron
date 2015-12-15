#include "lordaeron/ui/scene_node_inspector_pane.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/views/layout/fill_layout.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/ui/light_property_pane.h"
#include "lordaeron/ui/transform_property_pane.h"
#include "lordaeron/ui/scene_node_property_pane.h"

namespace lord {
using base::UTF8ToUTF16;
const char SceneNodeInspectorPane::kViewClassName[] = "lord::SceneNodeInspectorPane";
SceneNodeInspectorPane::SceneNodeInspectorPane() 
    : node_(NULL),
      scrollview_(NULL) {
  ClearUI();
}

const char* SceneNodeInspectorPane::GetClassName() const {
  return kViewClassName;
}

void SceneNodeInspectorPane::ClearUI() {
  RemoveAllChildViews(true);
  container_ = new nelf::CollapseViewContainer;
  scrollview_ = dynamic_cast<views::ScrollView*>(container_->CreateParentIfNecessary());
  AddChildView(scrollview_);
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

gfx::Size SceneNodeInspectorPane::GetPreferredSize() const {
  int32 scroll_width = scrollview_->GetScrollBarWidth();
  int32 scroll_height = scrollview_->GetScrollBarHeight();
  gfx::Insets insets = std::move(GetInsets());
  gfx::Size size = container_->GetPreferredSize();
  size.set_width(size.width() + scroll_width);
  size.set_width(size.width() + insets.width());

  size.set_height(size.height() + insets.height());
  if (parent()) {
    size.set_width(std::max(size.width(), parent()->GetContentsBounds().width()));
  }
  
  return size;
}

void SceneNodeContents::ChildPreferredSizeChanged(views::View* child) {
  SizeToPreferredSize();
}

void SceneNodeInspectorPane::Layout() {
  scrollview_->SetBoundsRect(std::move(GetContentsBounds()));
  int32 height = container_->GetPreferredSize().height();
  int32 width = scrollview_->GetContentsBounds().width();
  container_->SetSize(gfx::Size(width, height));
  scrollview_->Layout();
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
  SceneNodePropertyPane* node_pane = new SceneNodePropertyPane(node_);
  container_->AddChildView(node_pane);
  TransformPropertyPane* transform_pane = new TransformPropertyPane(node_);
  container_->AddChildView(transform_pane);
}

void SceneNodeInspectorPane::OnSceneNodeSelected(InteractiveContext* context, 
                                                 SceneNode* prevsel) {
  SetSceneNode(context->GetPickingNode());
  Layout();
  if (parent())
    parent()->Layout();
}

// class SceneNodeInspectorWindow
const char SceneNodeInspectorWindow::kViewClassName[] = 
    "nelf::SceneNodeInspectorWindow";
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

void SceneNodeInspectorWindow::Layout() {
  SizeToPreferredSize();
  views::View::Layout();
}
}  // namespace lord
