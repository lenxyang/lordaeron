#include <memory>

#include "lordaeron/sandbox/sandbox.h"
#include "lordaeron/sandbox/scene/scene_loader_delegate.h"

using views::Widget;
using namespace azer;

namespace lord {
class RendererInfoPane;
class MyRenderWindow : public SimpleRenderWindow {
 public:
  MyRenderWindow(const gfx::Rect& rect) : SimpleRenderWindow(rect) {}
  void OnInitScene() override;
  void OnInitUI() override;
  void OnUpdateFrame(const azer::FrameArgs& args) override;
  void OnRenderFrame(const azer::FrameArgs& args, Renderer* renderer) override;
  SceneNode* root() { return root_.get();}
 private:
  SceneNodePtr root_;
  DiffuseEffectPtr effect_;
  SceneRenderNodePtr render_root_;
  SceneRenderNodePtr bvolumn_root_;
  scoped_ptr<SimpleRenderTreeRenderer> tree_render_;
  scoped_ptr<SimpleRenderTreeRenderer> bvolumn_render_;
  scoped_ptr<azer::FPSCameraController> camera_controller_;
  scoped_ptr<FileSystem> fsystem_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};
}  // namespace lord

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));

  gfx::Rect init_bounds(0, 0, 800, 600);
  lord::MyRenderWindow* window(new lord::MyRenderWindow(init_bounds));
  nelf::ResourceBundle* bundle = lord::Context::instance()->resource_bundle();
  window->SetWindowIcon(*bundle->GetImageSkiaNamed(IDR_ICON_CAPTION_RULE));
  window->SetShowIcon(true);
  window->Init();
  window->Show();

  window->GetRenderLoop()->Run();
  return 0;
}

namespace lord {
using namespace azer;

void MyRenderWindow::OnInitScene() {
  effect_ = CreateDiffuseEffect();
  Context* ctx = Context::instance(); 
  fsystem_.reset(new NativeFileSystem(FilePath(FILE_PATH_LITERAL("lordaeron/"))));
  root_ = new SceneNode("root");

  VertexDescPtr desc = effect_->GetVertexDesc();
  scoped_ptr<SceneNodeLoader> light_loader(new LightNodeLoader());
  scoped_ptr<SceneNodeLoader> env_loader(new EnvNodeLoader());
  scoped_ptr<SimpleSceneLoaderDelegate> delegate(
      new SimpleSceneLoaderDelegate(effect_.get()));
  SceneLoader loader(fsystem_.get());
  loader.RegisterSceneNodeLoader(env_loader.Pass());
  loader.RegisterSceneNodeLoader(light_loader.Pass());
  loader.RegisterSceneNodeLoader(delegate.Pass());
  root_ = loader.Load(ResPath(UTF8ToUTF16("//sandbox/scene/scene.xml")), "//");
  CHECK(root_.get());

  {
    DefaultSceneRenderNodeCreator creator;
    SceneRenderTreeBuilder builder(&creator);
    builder.Build(root_.get(), &camera());
    render_root_ = builder.GetRenderNodeRoot();
    LOG(ERROR) << "\n" << render_root_->DumpTree();
    tree_render_.reset(new SimpleRenderTreeRenderer(render_root_.get()));
  }

  {
    SceneBVRenderNodeCreator creator;
    SceneRenderTreeBuilder builder(&creator);
    builder.Build(root_.get(), &camera());
    bvolumn_root_ = builder.GetRenderNodeRoot();
    bvolumn_render_.reset(new SimpleRenderTreeRenderer(bvolumn_root_.get()));
  }
}

void MyRenderWindow::OnInitUI() { 
  camera_controller_.reset(new FPSCameraController(mutable_camera()));
  view()->AddEventListener(camera_controller_.get());

  SceneTreeWindow* scene = new SceneTreeWindow(
      gfx::Rect(400, 300), this->window()->GetTopWindow());
  scene->SetSceneNode(root_);
  scene->Init();
  scene->Show();
  scene->SetTitle(base::UTF8ToUTF16("Scene"));
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  camera_controller_->Update(args);
  tree_render_->Update(args);
  bvolumn_render_->Update(args);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  tree_render_->Render(renderer);
  bvolumn_render_->Render(renderer);
}
}  // namespace lord
