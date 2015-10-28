#include <memory>

#include "base/strings/utf_string_conversions.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "azer/files/native_file_system.h"
#include "azer/render/util.h"
#include "ui/views/widget/widget_observer.h"
#include "nelf/nelf.h"
#include "nelf/gfx_util.h"
#include "nelf/res/grit/common.h"
#include "lordaeron/res/grit/common.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/effect/diffuse_effect_provider.h"
#include "lordaeron/effect/global_environemnt_params.h"
#include "lordaeron/mainframe_render_delegate.h"
#include "lordaeron/scene/scene_context.h"
#include "lordaeron/scene/scene_render.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"
#include "lordaeron/scene/scene_loader.h"
#include "lordaeron/ui/scene_tree_view.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/sandbox/scene/scene_loader_delegate.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/ui/renderer_info_pane.h"
#include "lordaeron/util/model_loader.h"

using views::Widget;
using lord::SceneNodePtr;
using lord::SceneNode;
using lord::SceneContext;
using lord::SceneContextPtr;
using base::UTF8ToUTF16;
using base::UTF16ToUTF8;
using namespace azer;

namespace lord {
class RendererInfoPane;
class MyRenderWindow : public lord::SceneRenderWindow {
 public:
  MyRenderWindow(const gfx::Rect& rect) : lord::SceneRenderWindow(rect) {}
  void OnInitScene() override;
  void OnInitUI() override;
  void OnUpdateFrame(const azer::FrameArgs& args) override;
  void OnRenderFrame(const azer::FrameArgs& args, Renderer* renderer) override;


  SceneNode* root() { return root_.get();}
 private:
  SceneNodePtr root_;
  SceneContextPtr scene_context_;
  scoped_ptr<SceneRender> scene_renderer_;
  lord::DiffuseEffectPtr effect_;
  azer::Matrix4 pv_;

  scoped_ptr<azer::FPSCameraController> camera_controller_;
  scoped_ptr<FileSystem> fsystem_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};
}  // namespace lord

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));

  gfx::Rect init_bounds(0, 0, 800, 600);
  lord::MyRenderWindow* window(new lord::MyRenderWindow(init_bounds));
  window->set_show_icon(true);
  nelf::ResourceBundle* bundle = lord::Context::instance()->resource_bundle();
  window->set_window_icon(*bundle->GetImageSkiaNamed(IDR_ICON_CAPTION_RULE));
  window->Init();
  window->Show();

  lord::ObjectControlToolbar* toolbar = new lord::ObjectControlToolbar(window);
  window->GetRenderLoop()->Run();
  return 0;
}

namespace lord {
using namespace azer;

void MyRenderWindow::OnInitScene() {
  effect_ = CreateDiffuseEffect();
  lord::Context* ctx = lord::Context::instance(); 
  fsystem_.reset(new azer::NativeFileSystem(
      ::base::FilePath(FILE_PATH_LITERAL("lordaeron/media"))));
  lord::DirLight dirlight;
  dirlight.dir = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  dirlight.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  dirlight.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
  LightPtr light(new Light(dirlight));
  scene_context_ = new lord::SceneContext;
  scene_context_->GetGlobalEnvironment()->SetCamera(mutable_camera());
  scene_context_->GetGlobalEnvironment()->SetLight(light);
  root_ = new SceneNode(scene_context_);

  std::string contents;
  base::ReadFileToString(base::FilePath(
      FILE_PATH_LITERAL("lordaeron/sandbox/scene/scene.sce")), &contents);
  ConfigNodePtr config_root = ConfigNode::InitFromXMLStr(contents);
  CHECK(config_root.get());

  VertexDescPtr desc = effect_->GetVertexDesc();
  SimpleSceneLoaderDelegate delegate(fsystem_.get(), effect_.get());
  SceneLoader loader(&delegate);
  CHECK(loader.Load(root_.get(), config_root));
  scene_renderer_.reset(new SceneRender(scene_context_.get(), root_.get()));
}

void MyRenderWindow::OnInitUI() { 
  camera_controller_.reset(new FPSCameraController(mutable_camera()));
  view()->AddEventListener(camera_controller_.get());

  lord::SceneTreeWindow* scene = new lord::SceneTreeWindow(
      gfx::Rect(400, 300), this->window()->GetTopWindow());
  scene->SetSceneNode(root_);
  scene->Init();
  scene->Show();
  scene->SetTitle(base::UTF8ToUTF16("Scene"));
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  camera_controller_->Update(args);
  pv_ = camera().GetProjViewMatrix();
  Renderer* renderer = window()->GetRenderer().get();
  scene_renderer_->Update(args);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  scene_renderer_->Render(renderer);
}
}  // namespace lord
