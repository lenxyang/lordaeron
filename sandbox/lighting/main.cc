#include <memory>

#include "lordaeron/sandbox/sandbox.h"
#include "lordaeron/sandbox/scene/scene_loader_delegate.h"
#include "lordaeron/sandbox/lighting/effect.h"
#include "lordaeron/sandbox/lighting/effect_adapter.h"

using views::Widget;
using lord::SceneNodePtr;
using lord::SceneNode;
using lord::SceneContext;
using lord::SceneContextPtr;
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
  SceneContextPtr scene_context_;
  scoped_ptr<SceneRender> scene_renderer_;
  sandbox::MyEffectPtr effect_;
  azer::Matrix4 pv_;

  scoped_ptr<azer::FPSCameraController> camera_controller_;
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

  window->GetRenderLoop()->Run();
  return 0;
}

namespace lord {
using namespace azer;

void MyRenderWindow::OnInitScene() {
  effect_ = sandbox::CreateMyEffect();
  Context* ctx = Context::instance(); 
  fsystem_.reset(new azer::NativeFileSystem(
      ::base::FilePath(FILE_PATH_LITERAL("lordaeron/media"))));
  DirLight dirlight;
  dirlight.direction = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  dirlight.diffuse = Vector4(0.8f, 0.8f, 0.8f, 1.0f);
  dirlight.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
  dirlight.specular = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
  LightPtr light(new Light(dirlight));
  scene_context_ = new SceneContext;
  scene_context_->GetGlobalEnvironment()->SetCamera(mutable_camera());
  scene_context_->GetGlobalEnvironment()->SetLight(light);
  root_ = new SceneNode(scene_context_);

  std::string contents;
  base::ReadFileToString(base::FilePath(
      FILE_PATH_LITERAL("lordaeron/sandbox/lighting/scene.sce")), &contents);
  ConfigNodePtr config_root = ConfigNode::InitFromXMLStr(contents);
  CHECK(config_root.get());

  VertexDescPtr desc = effect_->GetVertexDesc();
  scoped_ptr<SimpleSceneLoaderDelegate> delegate(
      new SimpleSceneLoaderDelegate(fsystem_.get(), effect_.get()));
  SceneLoader loader;
  loader.RegisterSceneNodeLoader(delegate.Pass());
  CHECK(loader.Load(root_.get(), config_root));
  scene_renderer_.reset(new SceneRender(scene_context_.get(), root_.get()));
}

void MyRenderWindow::OnInitUI() { 
  camera_controller_.reset(new FPSCameraController(mutable_camera()));
  view()->AddEventListener(camera_controller_.get());

  /*
  SceneTreeWindow* scene = new SceneTreeWindow(
      gfx::Rect(400, 300), this->window()->GetTopWindow());
  scene->SetSceneNode(root_);
  scene->Init();
  scene->Show();
  scene->SetTitle(base::UTF8ToUTF16("Scene"));
  */
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
