#include <memory>

#include "lordaeron/sandbox/sandbox.h"
#include "lordaeron/interactive/directional_light_object.h"
#include "lordaeron/sandbox/lighting/scene_loader_delegate.h"
#include "lordaeron/sandbox/lighting/effect.h"
#include "lordaeron/sandbox/lighting/effect_adapter.h"

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
  scoped_ptr<DirectionalLightObject> dirlight_controller_;
  sandbox::MyEffectPtr effect_;
  azer::Matrix4 pv_;

  scoped_ptr<FileSystem> fsystem_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};
}  // namespace lord

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));

  lord::Context* ctx = lord::Context::instance();
  azer::EffectAdapterContext* adapterctx = ctx->GetEffectAdapterContext();
  adapterctx->RegisteAdapter(new lord::sandbox::ColorEffectAdapter);
  adapterctx->RegisteAdapter(new lord::sandbox::SceneNodeColorEffectAdapter);
  adapterctx->RegisteAdapter(new lord::sandbox::GlobalEnvColorEffectAdapter);

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
  DirLight dirlight;
  dirlight.direction = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  dirlight.diffuse = Vector4(0.8f, 0.8f, 0.8f, 1.0f);
  dirlight.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
  dirlight.specular = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
  LightPtr light(new Light(dirlight));

  scene_context_ = new SceneContext;
  scene_context_->GetGlobalEnvironment()->SetCamera(mutable_camera());
  scene_context_->GetGlobalEnvironment()->SetLight(light);

  scoped_ptr<SceneNodeLoader> light_loader(new LightNodeLoader());
  scoped_ptr<SceneNodeLoader> env_loader(new EnvNodeLoader());
  scoped_ptr<SimpleSceneNodeLoader> node_loader(new SimpleSceneNodeLoader(
      fsystem_.get(), effect_.get()));
  SceneLoader loader(fsystem_.get(), scene_context_.get());
  loader.RegisterSceneNodeLoader(node_loader.Pass());
  loader.RegisterSceneNodeLoader(env_loader.Pass());
  loader.RegisterSceneNodeLoader(light_loader.Pass());
  SceneNodePtr root = loader.Load(ResPath(UTF8ToUTF16("//sandbox/lighting/scene.xml")), "//");
  scene_renderer_.reset(new SceneRender(scene_context_.get(), root.get()));
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
  pv_ = camera().GetProjViewMatrix();
  Renderer* renderer = window()->GetRenderer().get();
  scene_renderer_->Update(args);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  const Matrix4& pv = camera().GetProjViewMatrix();
  scene_renderer_->Render(renderer);
}
}  // namespace lord
