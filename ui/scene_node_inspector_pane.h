#pragma once

#include "ui/views/view.h"
#include "nelf/nelf.h"
#include "lordaeron/interactive/interactive_context.h"

namespace lord {
class SceneNode;
class SceneNodeInspectorPane;

class SceneNodeInspectorFootBar : public views::View {
 public:
  static const char kViewClassName[];
  explicit SceneNodeInspectorFootBar(SceneNodeInspectorPane* pane);

  const char* GetClassName() const override;
  gfx::Size GetPreferredSize() const override;
 private:
  SceneNodeInspectorPane* pane_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeInspectorFootBar);
};

class SceneNodeInspectorPane : public views::View,
                               public InteractiveContextObserver {
 public:
  static const char kViewClassName[];
  SceneNodeInspectorPane();

  SceneNode* GetSceneNode() { return node_;}
  const char* GetClassName() const override;
  void Layout() override;
  void SetSceneNode(SceneNode* node);
  gfx::Size GetPreferredSize() const override;
  void ChildPreferredSizeChanged(views::View* child) override;
  void OnSceneNodeSelected(InteractiveContext* context, SceneNode* prevsel) override;
 private:
  void ClearUI();
  void InitUIForLampNode();
  void InitUIForObjectNode();
  void InitUIForCommonNode();
  SceneNode* node_;
  nelf::CollapseViewContainer* container_;
  views::ScrollView* scrollview_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeInspectorPane);
};

class SceneNodeInspectorWindow : public nelf::TabbedWindow {
 public:
  static const char kViewClassName[];
  SceneNodeInspectorWindow(const gfx::Rect& bounds, nelf::MainFrame* mainframe);

  void SetSceneNode(SceneNode* node);
  const char* GetClassName() const override;
  void Layout() override;
  SceneNodeInspectorPane* inspector_pane() { return inspector_pane_;}
 private:
  SceneNodeInspectorPane* inspector_pane_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodeInspectorWindow);
};
}  // namespace lord
