#include <memory>

#include "lordaeron/sandbox/sandbox.h"
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
  Context* ctx = Context::instance();
  fsystem_.reset(new azer::NativeFileSystem(FilePath(UTF8ToUTF16("lordaeron/"))));

  ResourceLoader resloader(fsystem_.get());
  InitDefaultLoader(&resloader);
  ResPath respath(UTF8ToUTF16("//sandbox/lighting/scene.xml"));
  Resource res = resloader.Load(respath);
  SceneNodePtr root = res.scene;

  tree_render_.reset(new SimpleRenderTreeRenderer);
  LoadSceneRenderNodeDelegateFactory factory(tree_render_.get());
  SceneRenderTreeBuilder builder(&factory);
  render_root_ = builder.Build(root.get(), &camera());
  tree_render_->SetSceneNode(render_root_.get());
  LOG(ERROR) << "\n" << render_root_->DumpTree();
  
  return root;
}

void MyRenderWindow::OnInitUI() {
  gfx::Rect bounds(300, 360);
  nelf::TabbedWindow* wnd = CreateSceneTreeViewWindow(bounds, root(), this);
  wnd->Dock(nelf::kDockLeft);

  SceneNodeInspectorWindow* inspector = new SceneNodeInspectorWindow(bounds, this);
  inspector->Init();
  GetInteractive()->AddObserver(inspector->inspector_pane());
  inspector->Show();
  mutable_camera()->reset(Vector3(0.0f, 8.0f, 12.0f), Vector3(0.0f, 0.0f, 0.0f),
                          Vector3(0.0f, 1.0f, 0.0f));
  inspector->Dock(nelf::kDockLeft);
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  tree_render_->Update(args);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  tree_render_->Render(renderer);
}
}  // namespace lord
