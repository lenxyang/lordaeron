#pragma once

#include "ui/views/view.h"

namespace lord {
gfx::Rect LayoutCenterLeft(views::View* parent, views::View* view, 
                           int center_x, int y, int padding, 
                           const gfx::Size& size);
gfx::Rect LayoutCenterRight(views::View* parent, views::View* view, 
                            int center_x, int y, int padding, 
                            const gfx::Size& size);
gfx::Rect LayoutCenterLeftWithPrefSize(views::View* parent, views::View* view, 
                                       int center_x, int y, int padding);

gfx::Rect LayoutCenterRightWithPrefSize(views::View* parent, views::View* view, 
                                        int center_x, int y, int padding);
}  // namespace lord
