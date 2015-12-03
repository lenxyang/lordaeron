#include <memory>

#include "lordaeron/sandbox/sandbox.h"
#include "lordaeron/sandbox/lighting/scene_loader_delegate.h"
#include "lordaeron/sandbox/lighting/effect.h"
#include "lordaeron/sandbox/lighting/effect_adapter.h"

using base::FilePath;
using base::UTF8ToUTF16;
using views::Widget;
using lord::SceneNodePtr;
using lord::SceneNode;
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
  sandbox::MyEffectPtr effect_;

  SceneRenderNodePtr render_root_;
  SceneRenderNodePtr bvolumn_root_;
  scoped_ptr<SimpleRenderTreeRenderer> tree_render_;
  scoped_ptr<SimpleRenderTreeRenderer> bvolumn_render_;
  scoped_ptr<FileSystem> fsystem_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};
}  // namespace lord

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));

  lord::Context* ctx = lord::Context::instance();
  azer::EffectAdapterContext* adapterctx = ctx->GetEffectAdapterContext();
  adapterctx->RegisteAdapter(new lord::sandbox::ColorEffectAdapter);
  adapterctx->RegisteAdapter(new lord::sandbox::SceneRenderNodeEffectAdapter);
  adapterctx->RegisteAdapter(new lord::sandbox::SceneRenderEnvNodeEffectAdapter);

  gfx::Rect init_bounds(0, 0, 800, 600);
  lord::MyRenderWindow* window(new lord::MyRenderWindow(init_bounds));
  nelf::ResourceBundle* bundle = lord::Context::instance()->resource_bundle();
  window->SetWindowIcon(*bundle->GetImageSkiaNamed(IDR_ICON_CAPTION_RULE));
  window->SetShowIcon(true);
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
  effect_ = sandbox::CreateMyEffect();
  Context* ctx = Context::instance();
  fsystem_.reset(new azer::NativeFileSystem(FilePath(UTF8ToUTF16("lordaeron/"))));

  scoped_ptr<SceneNodeLoader> light_loader(new LightNodeLoader());
  scoped_ptr<SceneNodeLoader> env_loader(new EnvNodeLoader());
  scoped_ptr<SimpleSceneNodeLoader> node_loader(new SimpleSceneNodeLoader(
      fsystem_.get(), effect_.get()));
  SceneLoader loader(fsystem_.get());
  loader.RegisterSceneNodeLoader(node_loader.Pass());
  loader.RegisterSceneNodeLoader(env_loader.Pass());
  loader.RegisterSceneNodeLoader(light_loader.Pass());
  SceneNodePtr root = loader.Load(ResPath(UTF8ToUTF16("//sandbox/lighting/scene.xml")), "//");

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
