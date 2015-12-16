#include "lordaeron/ui/layout_util.h"
#include "ui/views/view.h"

namespace lord {
gfx::Rect LayoutCenterLeft(views::View* parent, views::View* view, 
                           int center_x, int y, int padding, 
                           const gfx::Size& size) {
  const gfx::Rect bounds = std::move(parent->GetContentsBounds());
  view->SetSize(size);
  int x = center_x - padding - size.width();
  view->SetPosition(gfx::Point(x, y));
  return view->bounds();
}

gfx::Rect LayoutCenterRight(views::View* parent, views::View* view, 
                            int center_x, int y, int padding, 
                            const gfx::Size& size) {
  const gfx::Rect bounds = std::move(parent->GetContentsBounds());
  view->SetSize(size);
  int x = center_x + padding;
  view->SetPosition(gfx::Point(x, y));
  return view->bounds();
}

gfx::Rect LayoutCenterLeftWithPrefSize(views::View* parent, views::View* view, 
                                       int center_x, int y, int padding) {
  return LayoutCenterLeft(parent, view, center_x, y, 
                          padding, view->GetPreferredSize());
}

gfx::Rect LayoutCenterRightWithPrefSize(views::View* parent, views::View* view, 
                                       int center_x, int y, int padding) {
  return LayoutCenterRight(parent, view, center_x, y, 
                           padding, view->GetPreferredSize());
}
}  // namespace lord
