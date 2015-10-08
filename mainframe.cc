#include <memory>

#include "base/strings/utf_string_conversions.h"
#include "nelf/nelf.h"
#include "nelf/gfx_util.h"
#include "nelf/res/grit/common.h"
#include "lordaeron/context.h"
#include "lordaeron/ui/scene_tree_view.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/ui/render_frame_window.h"
#include "lordaeron/mainframe_render_delegate.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"
#include "azer/render/util.h"

using views::Widget;
using namespace azer;

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));
  scoped_ptr<nelf::RenderDelegate> delegate(new lord::MainframeRenderDelegate);
  lord::RenderFrameWindow* window = new lord::RenderFrameWindow(
      gfx::Rect(0, 0, 800, 600), delegate.Pass());
  window->Show();

  lord::SceneNodePtr root(new lord::SceneNode("root"));
  root->AddChild(lord::SceneNodePtr(new lord::SceneNode("lamp1")));
  root->AddChild(lord::SceneNodePtr(new lord::SceneNode("lamp2")));
  root->AddChild(lord::SceneNodePtr(new lord::SceneNode("lamp3")));
  lord::SceneTreeWindow* scene = new lord::SceneTreeWindow(window, root);
  scene->Init();
  scene->Show();

  lord::ObjectControlToolbar* toolbar = new lord::ObjectControlToolbar(window);
  window->GetRenderLoop()->Run();
  return 0;
}


