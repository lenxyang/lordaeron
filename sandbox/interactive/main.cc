#include <memory>

#include "lordaeron/sandbox/sandbox.h"
#include "lordaeron/sandbox/interactive/scene_node_loader.h"
#include "lordaeron/interactive/fps_camera_controller.h"

using base::FilePath;
using base::UTF8ToUTF16;
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
  SceneNodePtr root_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};
}  // namespace lord

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));

  gfx::Rect init_bounds(0, 0, 800, 600);
  lord::MyRenderWindow* window(new lord::MyRenderWindow(init_bounds));
  window->SetShowIcon(true);
  nelf::ResourceBundle* bundle = lord::Context::instance()->resource_bundle();
  window->SetWindowIcon(*bundle->GetImageSkiaNamed(IDR_ICON_CAPTION_RULE));
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
  fsystem_.reset(new azer::NativeFileSystem(FilePath(UTF8ToUTF16("lordaeron/"))));
  lord::DirLight dirlight;
  dirlight.direction = Vector3(-0.6f, -0.2f, -0.2f);
  dirlight.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  dirlight.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
  LightPtr light(new Light(dirlight));
  scene_context_ = new lord::SceneContext;
  scene_context_->GetGlobalEnvironment()->SetCamera(mutable_camera());
  scene_context_->GetGlobalEnvironment()->SetLight(light);


  // load loader
  scoped_ptr<SceneNodeLoader> light_loader(new LightNodeLoader());
  scoped_ptr<SceneNodeLoader> env_loader(new EnvNodeLoader());
  scoped_ptr<SimpleSceneNodeLoader> node_loader(new SimpleSceneNodeLoader(
      fsystem_.get(), effect_.get()));
  SceneLoader loader(fsystem_.get(), scene_context_.get());
  loader.RegisterSceneNodeLoader(node_loader.Pass());
  loader.RegisterSceneNodeLoader(env_loader.Pass());
  loader.RegisterSceneNodeLoader(light_loader.Pass());
  SceneNodePtr root = loader.Load(ResPath(UTF8ToUTF16("//sandbox/interactive/scene.xml")), "//");
  scene_renderer_.reset(new SceneRender(scene_context_.get(), root.get()));
  return root;
}

void MyRenderWindow::OnInitUI() { 
  DCHECK(root());
  SceneTreeView* view = new SceneTreeView(root());
  nelf::Pane* pane = new nelf::Pane();
  pane->GetContents()->AddChildView(view);
  pane->GetContents()->SetLayoutManager(new views::FillLayout);
  pane->SetTitle(::base::UTF8ToUTF16("scene"));
  gfx::Rect bounds(400, 300);
  nelf::TabbedWindow* window = new nelf::TabbedWindow(bounds, this);
  window->AddPane(pane);
  window->Init();
  window->Show();
  window->Dock(nelf::kDockLeft);

  /*
  {
    Light* light = scene_context_->GetGlobalEnvironment()->light();
    DirectionalLightPane* view = new DirectionalLightPane();
    view->SetControlLight(light);
    nelf::Pane* pane = new nelf::Pane();
    pane->AddChildView(view);
    pane->SetLayoutManager(new views::FillLayout);
    pane->set_title(::base::UTF8ToUTF16("Directional Light"));
    gfx::Rect bounds(350, 210);
    nelf::TabbedWindow* window = new nelf::TabbedWindow(bounds, this);
    window->Init();
    window->AddPane(pane);
    window->Show();
  }
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
