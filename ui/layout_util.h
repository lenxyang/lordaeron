#pragma once

#include "ui/views/view.h"

namespace lord {
inline int LayoutControlRightAlign(views::View* parent, views::View* view, int32 y) {
  const gfx::Rect bounds = std::move(parent->GetContentsBounds());
  view->SizeToPreferredSize();
  view->SetPosition(gfx::Point(bounds.right() - view->width(), y));
  return view->bounds().bottom(); 
}

inline int LayoutControlLeftAlign(views::View* parent, views::View* view, int32 y) {
  const gfx::Rect bounds = std::move(parent->GetContentsBounds());
  view->SizeToPreferredSize();
  view->SetPosition(gfx::Point(bounds.x(), y));
  return view->bounds().bottom(); 
}
}  // namespace lord
