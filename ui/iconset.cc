#include "lordaeron/ui/iconset.h"

#include "nelf/nelf.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"

namespace lord {
Iconset::Iconset(Context* context) {
  nelf::IconsetPackage::Options opt;
  opt.margin = gfx::Insets(9, 4, 3, 51);
  opt.unit = 16;
  opt.border = 1;
  opt.padding = 2;
  const gfx::ImageSkia* img = context->resource_bundle()->GetImageSkiaNamed(
      IDR_ICONSET_BLENDER);
  package_.reset(new nelf::IconsetPackage(*img, opt));
}

gfx::ImageSkia Iconset::GetIcon(int32 id) {
  int32 x = (id & 0xFFFf0000) >> 16; 
  int32 y = id & 0x0000FFFF;
  return package_->GetIcon(x, y);
}
}  // namespace lord
