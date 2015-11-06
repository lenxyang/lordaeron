#include <memory>

#include "lordaeron/sandbox/sandbox.h"
#include "lordaeron/sandbox/interactive/scene_node_loader.h"
#include "lordaeron/interactive/fps_camera_controller.h"

using views::Widget;
using lord::SceneNodePtr;
using lord::SceneNode;
using lord::SceneContext;
using lord::SceneContextPtr;
using namespace azer;

namespace lord {
class RendererInfoPane;
class MyRenderWindow : public lord::SceneRenderWindow {
 public:
  MyRenderWindow(const gfx::Rect& rect) : lord::SceneRenderWindow(rect) {}
  SceneNodePtr OnInitScene() override;
  void OnInitUI() override;
  void OnUpdateFrame(const azer::FrameArgs& args) override;
  void OnRenderFrame(const azer::FrameArgs& args, Renderer* renderer) override;
 private:
  SceneContextPtr scene_context_;
  scoped_ptr<SceneRender> scene_renderer_;
  lord::DiffuseEffectPtr effect_;
  azer::Matrix4 pv_;
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

  lord::ObjectControlToolbar* toolbar =
      new lord::ObjectControlToolbar(window, window->GetInteractive());
  window->GetRenderLoop()->Run();
  return 0;
}

namespace lord {
using namespace azer;

SceneNodePtr MyRenderWindow::OnInitScene() {
  Vector3 camera_pos(10.0f, 10.0f, 10.0f);
  Vector3 lookat(0.0f, 0.0f, 0.0f);
  Vector3 up(0.0f, 1.0f, 0.0f);
  mutable_camera()->reset(camera_pos, lookat, up);

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
  SceneNodePtr root = new SceneNode(scene_context_);

  std::string contents;
  base::ReadFileToString(base::FilePath(
      FILE_PATH_LITERAL("lordaeron/sandbox/interactive/scene.xml")), &contents);
  ConfigNodePtr config_root = ConfigNode::InitFromXMLStr(contents);
  CHECK(config_root.get());

  VertexDescPtr desc = effect_->GetVertexDesc();
  scoped_ptr<LightNodeLoader> light_loader(new LightNodeLoader());
  scoped_ptr<SimpleSceneNodeLoader> node_loader(new SimpleSceneNodeLoader(
      fsystem_.get(), effect_.get()));
  SceneLoader loader;
  loader.RegisterSceneNodeLoader(node_loader.Pass());
  loader.RegisterSceneNodeLoader(light_loader.Pass());
  CHECK(loader.Load(root.get(), config_root));
  scene_renderer_.reset(new SceneRender(scene_context_.get(), root.get()));
  return root;
}

void MyRenderWindow::OnInitUI() { 
  DCHECK(root());
  /*
  lord::SceneTreeWindow* scene = new lord::SceneTreeWindow(
      gfx::Rect(400, 300), this->window()->GetTopWindow());
  scene->SetSceneNode(root());
  scene->Init();
  scene->Show();
  scene->SetTitle(base::UTF8ToUTF16("Scene"));
  */
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  pv_ = camera().GetProjViewMatrix();
  Renderer* renderer = window()->GetRenderer().get();
  scene_renderer_->Update(args);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  scene_renderer_->Render(renderer);
}
}  // namespace lord
