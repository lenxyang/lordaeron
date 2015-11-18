#pragma once

#include "base/strings/string16.h"
#include "ui/views/view.h"
#include "ui/views/controls/label.h"

namespace lord {
class CollapseView;

class CollapseHead : public views::View {
 public:
  static const char kViewClassName[];
  CollapseHead(CollapseView* view);
  ~CollapseHead();
  
  void SetText(const base::string16& text);
  
  const char* GetClassName() const override;
  void Layout() override;
  gfx::Size GetPreferredSize() const override;
  bool OnMousePressed(const ui::MouseEvent& event) override;
 private:
  CollapseView* collapse_;
  views::Label* label_;
  static const SkColor kBorderColor;
  static const SkColor kBackgroundColor;
  static const SkColor kTextColor;
  static const int32 kHeight = 16;
  DISALLOW_COPY_AND_ASSIGN(CollapseHead);
};

class CollapseBorderView : public views::View {
 public:
  static const char kViewClassName[];
  CollapseBorderView();
  ~CollapseBorderView();
 private:
  static const SkColor kBorderColor;
  static const SkColor kBorderShadowColor;
  DISALLOW_COPY_AND_ASSIGN(CollapseBorderView);
};

class CollapseView : public views::View {
 public:
  static const char kViewClassName[];
  CollapseView();
  ~CollapseView();

  
  bool is_collapsed() const;
  void Collapse();
  void Expand();

  const char* GetClassName() const override;
  void Layout() override;
  gfx::Size GetPreferredSize() const override;  
 private:
  CollapseBorderView* border_view_;
  CollapseHead* head_;
  views::View* contents_;
  DISALLOW_COPY_AND_ASSIGN(CollapseView);
};
}  // namespace lord
