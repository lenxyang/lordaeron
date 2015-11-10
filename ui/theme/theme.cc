#include "lordaeron/ui/theme/theme.h"

#include "nelf/theme/five_pic_nonclient_frame_theme.h"
#include "lordaeron/context.h"
#include "lordaeron/res/grit/common.h"

namespace lord {
namespace {
class LordaeronWindowTheme : public nelf::FallbackWindowTheme {
 public:
  LordaeronWindowTheme(nelf::Context* context)
      : FallbackWindowTheme(context) {
  }

  nelf::WindowNonClientFrame* CreateWindowNonClient(nelf::Window* window) {
    using namespace nelf;
    nelf::WindowNonClientFrame* frame = nelf::FallbackWindowTheme::
        CreateWindowNonClient(window);
    lord::Context* ctx = lord::Context::instance();
    nelf::ResourceBundle* bundle = ctx->resource_bundle();
    ImageVector images;
    images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_TOP));
    images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_BOTTOM));
    images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_LEFT));
    images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_RIGHT));
    images.push_back(bundle->GetImageSkiaNamed(IDR_WINDOW_BG));
    scoped_ptr<FivePicNonClientFramePainter> painter(
        new FivePicNonClientFramePainter(images));
    FivePicNonClientFrameTheme* theme = new FivePicNonClientFrameTheme(window);
    theme->SetPainter(painter.Pass());

    frame->SetTheme(theme);
    return frame;
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(LordaeronWindowTheme);
};
}

Theme::Theme(nelf::Context* context)
    : FallbackTheme(context) {
}

Theme::~Theme() {
}

scoped_ptr<nelf::WindowTheme> Theme::CreateWindowTheme(nelf::Window* window) {
  scoped_ptr<LordaeronWindowTheme> theme(new LordaeronWindowTheme(context()));
  return theme.Pass();
}
}  // namespace lord
