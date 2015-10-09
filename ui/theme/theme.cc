#include "lordaeron/ui/theme/theme.h"

#include "nelf/theme/five_pic_nonclient_frame_theme.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"

namespace lord {
Theme::Theme(nelf::Context* context)
    : FallbackTheme(context) {
}

Theme::~Theme() {
}

nelf::NonClientFrameThemePtr Theme::CreateWindowTheme(nelf::Window* window) {
  using namespace nelf;
  lord::Context* ctx = lord::Context::instance();
  nelf::ResourceBundle* bundle = ctx->resource_bundle();
  ImageVector images;
  images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_TOP));
  images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_BOTTOM));
  images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_LEFT));
  images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_RIGHT));
  images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_BG));
  scoped_ptr<FivePicNonClientFramePainter> painter(new FivePicNonClientFramePainter(images));
  FivePicNonClientFrameTheme* theme = new FivePicNonClientFrameTheme(window);
  theme->SetPainter(painter.Pass());
  return NonClientFrameThemePtr(theme);
}
}  // namespace lord
