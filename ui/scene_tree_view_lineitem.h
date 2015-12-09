#pragma once

#include "nelf/nelf.h"

namespace lord {
class SceneNodeControPane;
class SceneTreeViewLineItemView : public nelf::CollapsedBasedTreeView::LineItemView {
 public:
  static const char kViewClassName[];
  SceneTreeViewLineItemView(nelf::CollapsedBasedTreeViewNode* node);
  void OnNodeSelected(bool selected) override;
  void OnNodeChanged() override;

  // override from EditingHelperDelegate
  void LayoutEditor(views::Textfield* editor) override;
  void OnEditBegin(views::Textfield* editor) override;
  void OnEditEnd(views::Textfield* editor, bool commit) override;

  // override from views::View
  const char* GetClassName() const override;
  void Layout() override;
  void OnPaint(gfx::Canvas* canvas) override;
  gfx::Size GetPreferredSize() const override; 
  bool OnMousePressed(const ui::MouseEvent& event) override;
  void OnNativeThemeChanged(const ui::NativeTheme* nm)  override;
  void PreferredSizeChanged() override;
  void OnBoundsChanged(const gfx::Rect& previous_bounds) override;
  void OnVisibleBoundsChanged() override;
  void VisibilityChanged(View* starting_from, bool is_visible) override;
 private:
  int32 CalcLeftMargin() const;
  void ExpandOrCollapse();
  void UpdateNodeInfo();
  
  void CalcLayout();
  gfx::ImageSkia icon_;
  nelf::CollapsedBasedTreeViewNode* node_;
  gfx::Size preferred_size_;
  gfx::Rect expand_bounds_;
  gfx::Rect icon_bounds_;
  gfx::Rect text_bounds_;
  SceneNodeControPane* control_panel_;
  
  int32 line_height_;
  static const int32 kTextVerticalPadding;
  static const int32 kImageMargin;
  static const int32 kIdent;
  DISALLOW_COPY_AND_ASSIGN(SceneTreeViewLineItemView);
};

class SceneTreeViewLineItemViewCreator 
    : public nelf::CollapsedBasedTreeView::LineItemViewCreator {
 public:
  SceneTreeViewLineItemViewCreator() {}
  nelf::CollapsedBasedTreeView::LineItemView* CreateNode(
      nelf::CollapsedBasedTreeViewNode* node) override {
    return new SceneTreeViewLineItemView(node);
  }
};
}  // namespace nelf
