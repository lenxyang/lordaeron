#pragma once

#include "ui/views/view.h"
#include "nelf/nelf.h"

namespace lord {
class SceneNode;
class SceneNodePropertyPane : public views::View {
 public:
  static const char kViewClassName[];
  SceneNodePropertyPane();

  const char* GetClassName() const override;
  void SetSceneNode(SceneNode* node);
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
  explicit SceneNodePropertyWindow(nelf::MainFrame* mainframe);

  void SetSceneNode(SceneNode* node);
  const char* GetClassName() const override;
 private:
  SceneNodePropertyPane* property_pane_;
  DISALLOW_COPY_AND_ASSIGN(SceneNodePropertyWindow);
};
}  // namespace lord
