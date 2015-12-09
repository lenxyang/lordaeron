#pragma once

#include "ui/views/view.h"
#include "nelf/nelf.h"
#include "lordaeron/interactive/interactive_context.h"

namespace lord {
class SceneNode;
class SceneNodePropertyPane : public views::View,
                              public InteractiveContextObserver {
 public:
  static const char kViewClassName[];
  SceneNodePropertyPane();

  const char* GetClassName() const override;
  void Layout() override;
  void SetSceneNode(SceneNode* node);
  void OnSceneNodeSelected(InteractiveContext* context, SceneNode* prevsel) override;
 private:
  void ClearUI();
  void InitUIForLampNode();
  void InitUIForObjectNode();
  void InitUIForCommonNode();
  SceneNode* node_;
  nelf::CollapseViewContainer* container_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodePropertyPane);
};

class SceneNodePropertyWindow : public nelf::TabbedWindow {
 public:
  static const char kViewClassName[];
  SceneNodePropertyWindow(const gfx::Rect& bounds, nelf::MainFrame* mainframe);

  void SetSceneNode(SceneNode* node);
  const char* GetClassName() const override;
  SceneNodePropertyPane* property_pane() { return property_pane_;}
 private:
  SceneNodePropertyPane* property_pane_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodePropertyWindow);
};
}  // namespace lord
