#include <memory>

#include "base/strings/utf_string_conversions.h"
#include "nelf/nelf.h"
#include "nelf/gfx_util.h"
#include "nelf/res/grit/common.h"
#include "lordaeron/res/grit/common.h"
#include "lordaeron/context.h"
#include "lordaeron/ui/scene_tree_view.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/ui/render_frame_window.h"
#include "lordaeron/mainframe_render_delegate.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"
#include "azer/render/util.h"

using views::Widget;
using lord::SceneNodePtr;
using lord::SceneNode;
using lord::SceneContext;
using lord::SceneContextPtr;
using namespace azer;

MeshPtr CreateSphere() {
  RenderSystem* rs = RenderSystem::Current();
  ColoredDiffuseEffectPtr diffuse_effect = CreateColoredDiffuseEffect();
  GeometryObjectPtr obj = new CylinderObject(diffuse_effect->GetVertexDesc());
  MeshPtr mesh = new Mesh(&context_);

  Mesh::Entity entity;
  entity.effect = diffuse_effect;
  entity.vb = obj->GetVertexBuffer();
  entity.ib = obj->GetIndicesBuffer();
  entity.provider = new MaterialEffectProvider();
  entity.adapter = new MaterialEffectAdapter;
  mesh->AddEntity(entity);
}

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));
  scoped_ptr<nelf::RenderDelegate> delegate(new lord::MainframeRenderDelegate);
  lord::RenderFrameWindow* window = new lord::RenderFrameWindow(
      gfx::Rect(0, 0, 800, 600), delegate.Pass());
  window->set_show_icon(true);
  nelf::ResourceBundle* bundle = lord::Context::instance()->resource_bundle();
  window->set_window_icon(*bundle->GetImageSkiaNamed(IDR_ICON_CAPTION_RULE));
  window->Show();

  SceneContextPtr scene_context(new SceneContext);
  SceneNodePtr root(new SceneNode(scene_context));
  root->set_name("root");
  SceneNodePtr obj1(new SceneNode("obj1"));
  SceneNodePtr obj2(new SceneNode("obj1"));
  SceneNodePtr obj3(new SceneNode("obj1"));
  root->AddChild(obj1);
  root->AddChild(obj2);
  root->AddChild(obj3);

  azer::ColoredDiffuseEffectPtr effect = azer::CreateColoredDiffuseEffect();
  GeometryObjectPtr object(new ConeObject(effect->GetVertexDesc()));

  lord::ObjectControlToolbar* toolbar = new lord::ObjectControlToolbar(window);
  window->GetRenderLoop()->Run();
  return 0;
}


