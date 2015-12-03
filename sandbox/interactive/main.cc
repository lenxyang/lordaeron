#include <memory>

#include "lordaeron/sandbox/sandbox.h"
#include "lordaeron/sandbox/interactive/scene_node_loader.h"
#include "lordaeron/interactive/fps_camera_controller.h"

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
  lord::DiffuseEffectPtr effect_;
  scoped_ptr<FileSystem> fsystem_;
  SceneNodePtr root_;
  SceneRenderNodePtr render_root_;
  SceneRenderNodePtr bvolumn_root_;
  scoped_ptr<SimpleRenderTreeRenderer> tree_render_;
  scoped_ptr<SimpleRenderTreeRenderer> bvolumn_render_;
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

  // load loader
  scoped_ptr<SceneNodeLoader> light_loader(new LightNodeLoader());
  scoped_ptr<SceneNodeLoader> env_loader(new EnvNodeLoader());
  scoped_ptr<SimpleSceneNodeLoader> node_loader(new SimpleSceneNodeLoader(
      fsystem_.get(), effect_.get()));
  SceneLoader loader(fsystem_.get());
  loader.RegisterSceneNodeLoader(node_loader.Pass());
  loader.RegisterSceneNodeLoader(env_loader.Pass());
  loader.RegisterSceneNodeLoader(light_loader.Pass());
  SceneNodePtr root = loader.Load(ResPath(UTF8ToUTF16("//sandbox/interactive/scene.xml")), "//");
  {
    DefaultSceneRenderNodeCreator creator;
    SceneRenderTreeBuilder builder(&creator);
    builder.Build(root.get(), &camera());
    render_root_ = builder.GetRenderNodeRoot();
    LOG(ERROR) << "\n" << render_root_->DumpTree();
    tree_render_.reset(new SimpleRenderTreeRenderer(render_root_.get()));
  }

  {
    SceneBVRenderNodeCreator creator;
    SceneRenderTreeBuilder builder(&creator);
    builder.Build(root.get(), &camera());
    bvolumn_root_ = builder.GetRenderNodeRoot();
    bvolumn_render_.reset(new SimpleRenderTreeRenderer(bvolumn_root_.get()));
  }
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
  tree_render_->Update(args);
  bvolumn_render_->Update(args);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  tree_render_->Render(renderer);
  bvolumn_render_->Render(renderer);
}
}  // namespace lord
