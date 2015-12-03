#include <memory>
#include "lordaeron/sandbox/sandbox.h"
#include "lordaeron/sandbox/scene/scene_loader_delegate.h"

using views::Widget;
using namespace azer;

namespace lord {
class RendererInfoPane;
class MyRenderWindow : public lord::SimpleRenderWindow {
 public:
  MyRenderWindow(const gfx::Rect& rect) : lord::SimpleRenderWindow(rect) {}
  void OnInitScene() override;
  void OnInitUI() override;
  void OnUpdateFrame(const azer::FrameArgs& args) override;
  void OnRenderFrame(const azer::FrameArgs& args, Renderer* renderer) override;

  SceneNode* root() { return root_.get();}
 private:
  SceneNodePtr root_;
  lord::DiffuseEffectPtr effect_;
  scoped_ptr<azer::FPSCameraController> camera_controller_;
  scoped_ptr<FileSystem> fsystem_;
  SceneRenderNodePtr render_root_;
  scoped_ptr<SimpleRenderTreeRenderer> tree_render_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};
}  // namespace lord

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));

  gfx::Rect init_bounds(0, 0, 800, 600);
  lord::MyRenderWindow* window(new lord::MyRenderWindow(init_bounds));
  nelf::ResourceBundle* bundle = lord::Context::instance()->resource_bundle();
  window->Init();
  window->Show();

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
  dirlight.direction = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  dirlight.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  dirlight.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
  LightPtr light(new Light(dirlight));
  root_ = new SceneNode("root");
  SceneNodePtr scene1(new SceneNode("scene1", root_.get()));
  SceneNodePtr envnode(new SceneNode("env", kEnvSceneNode, scene1.get()));
  SceneNodePtr lightnode(new SceneNode("dirlight", envnode.get()));
  lightnode->mutable_data()->AttachLight(light.get());
  lightnode->SetPosition(Vector3(8.0f, 8.0f, 0.0f));
  SceneNodePtr node1(new SceneNode("obj1", scene1.get()));
  SceneNodePtr node2(new SceneNode("obj2", scene1.get()));
  SceneNodePtr node3(new SceneNode("obj3", scene1.get()));

  lord::ModelLoader loader(fsystem_.get());
  VertexDescPtr desc = effect_->GetVertexDesc();
  lord::DiffuseColorProvider* p1(new lord::DiffuseColorProvider);
  lord::DiffuseColorProvider* p2(new lord::DiffuseColorProvider);
  lord::DiffuseColorProvider* p3(new lord::DiffuseColorProvider);
  p1->SetColor(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
  p2->SetColor(azer::Vector4(0.6f, 0.6f, 0.6f, 1.0f));
  p3->SetColor(azer::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
  azer::MeshPtr obj1 = loader.Load(ResPath(UTF8ToUTF16("//model/teapot.obj")), desc);
  azer::MeshPtr obj2 = loader.Load(ResPath(UTF8ToUTF16("//model/trunk.obj")), desc);
  azer::MeshPtr obj3 = loader.Load(ResPath(UTF8ToUTF16("//model/cow.obj")), desc);
  obj1->SetEffectAdapterContext(ctx->GetEffectAdapterContext());
  obj2->SetEffectAdapterContext(ctx->GetEffectAdapterContext());
  obj3->SetEffectAdapterContext(ctx->GetEffectAdapterContext());
  obj1->AddProvider(EffectParamsProviderPtr(p1));
  obj2->AddProvider(EffectParamsProviderPtr(p2));
  obj3->AddProvider(EffectParamsProviderPtr(p3));

  InitMeshEffect(effect_.get(), obj1.get());
  InitMeshEffect(effect_.get(), obj2.get());
  InitMeshEffect(effect_.get(), obj3.get());
  node1->mutable_data()->AttachMesh(obj1);
  node2->mutable_data()->AttachMesh(obj2);
  node3->mutable_data()->AttachMesh(obj3);
  node1->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  node2->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
  node3->SetPosition(Vector3(10.0f, 00.0f, 0.0f));
  node3->set_draw_bounding_volumn(true);

  DefaultSceneRenderNodeCreator creator;
  SceneRenderTreeBuilder builder(&creator);
  builder.Build(root_.get(), &camera());
  render_root_ = builder.GetRenderNodeRoot();
  LOG(ERROR) << "\n" << render_root_->DumpTree();
  tree_render_.reset(new SimpleRenderTreeRenderer(render_root_.get()));
}

void MyRenderWindow::OnInitUI() { 
  camera_controller_.reset(new FPSCameraController(mutable_camera()));
  view()->AddEventListener(camera_controller_.get());
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  camera_controller_->Update(args);
  tree_render_->Update(args);
}

void MyRenderWindow::OnRenderFrame(const azer::FrameArgs& args, Renderer* renderer) {
  tree_render_->Render(renderer);
}
}  // namespace lord
