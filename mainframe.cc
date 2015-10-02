#include <memory>

#include "base/strings/utf_string_conversions.h"
#include "nelf/nelf.h"
#include "nelf/gfx_util.h"
#include "nelf/res/grit/common.h"
#include "lordaeron/context.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/ui/render_frame_window.h"
#include "lordaeron/mainframe_render_delegate.h"
#include "azer/render/util.h"

using views::Widget;
using namespace azer;

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));
  scoped_ptr<nelf::RenderDelegate> delegate(new MainframeRenderDelegate);
  lord::RenderFrameWindow* window = new lord::RenderFrameWindow(
      gfx::Rect(0, 0, 800, 600), delegate.Pass());
  window->Show();

  lord::ObjectControlToolbar* toolbar = new lord::ObjectControlToolbar(window);
  window->GetRenderLoop()->Run();
  return 0;
}


