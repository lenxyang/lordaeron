#include <memory>

#include "base/strings/utf_string_conversions.h"
#include "base/files/file_path.h"
#include "azer/files/native_file_system.h"
#include "azer/render/util.h"
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
#include "lordaeron/ui/scene_tree_view.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/ui/render_frame_window.h"
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
class RenderDelegate : public nelf::RenderDelegate {
 public:
  RenderDelegate();
  virtual bool Initialize() override;
  virtual void OnUpdate(const azer::FrameArgs& args) override;
  virtual void OnRender(const azer::FrameArgs& args) override;

  // attributes
  const azer::Camera& camera() const { return camera_;}
  void InitScene();
 private:
  SceneNodePtr root_;
  SceneContextPtr scene_context_;
  scoped_ptr<SceneRender> scene_renderer_;

  lord::DiffuseEffectPtr effect_;
  azer::Camera camera_;
  azer::Matrix4 pv_;
  RendererInfoPane* renderer_pane_;
  scoped_ptr<CameraOverlay> camera_overlay_;
  scoped_ptr<azer::CoordinateGrid> gridline_;
  scoped_ptr<azer::FPSCameraController> camera_controller_;
  scoped_ptr<FileSystem> fsystem_;
  DISALLOW_COPY_AND_ASSIGN(RenderDelegate);
};
}  // namespace lord

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));
  scoped_ptr<nelf::RenderDelegate> delegate(new lord::RenderDelegate);
  lord::RenderFrameWindow* window = new lord::RenderFrameWindow(
      gfx::Rect(0, 0, 800, 600), delegate.Pass());
  window->set_show_icon(true);
  nelf::ResourceBundle* bundle = lord::Context::instance()->resource_bundle();
  window->set_window_icon(*bundle->GetImageSkiaNamed(IDR_ICON_CAPTION_RULE));
  window->Show();

  lord::ObjectControlToolbar* toolbar = new lord::ObjectControlToolbar(window);
  window->GetRenderLoop()->Run();
  return 0;
}

namespace lord {
using namespace azer;

const Vector4 kGridLineColor = Vector4(0.663f, 0.663f, 0.663f, 1.0f);
const Vector4 kRenderBgColor = Vector4(0.427f, 0.427f, 0.427f, 1.0f);

RenderDelegate::RenderDelegate()
    : renderer_pane_(NULL) {
}


void RenderDelegate::InitScene() {
  lord::Context* ctx = lord::Context::instance(); 
  fsystem_.reset(new azer::NativeFileSystem(
      ::base::FilePath(FILE_PATH_LITERAL("lordaeron/media"))));
  lord::DirLight dirlight;
  dirlight.dir = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  dirlight.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  dirlight.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
  LightPtr light(new Light(dirlight));
  scene_context_ = new lord::SceneContext;
  scene_context_->GetGlobalEnvironment()->SetCamera(&camera_);
  scene_context_->GetGlobalEnvironment()->SetLight(light);
  root_ = new SceneNode(scene_context_);
  root_->set_name("root");
  SceneNodePtr scene1(new SceneNode("scene1"));
  SceneNodePtr node1(new SceneNode("obj1"));
  SceneNodePtr node2(new SceneNode("obj1"));
  SceneNodePtr node3(new SceneNode("obj1"));
  root_->AddChild(scene1);
  scene1->AddChild(node1);
  scene1->AddChild(node2);
  scene1->AddChild(node3);

  lord::ModelLoader loader(fsystem_.get());
                 
  VertexDescPtr desc = effect_->GetVertexDesc();
  lord::DiffuseEffectProvider* p1(new lord::DiffuseEffectProvider);
  lord::DiffuseEffectProvider* p2(new lord::DiffuseEffectProvider);
  lord::DiffuseEffectProvider* p3(new lord::DiffuseEffectProvider);

  p1->SetColor(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
  p2->SetColor(azer::Vector4(0.6f, 0.6f, 0.6f, 1.0f));
  p3->SetColor(azer::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
  azer::MeshPtr obj1 = loader.Load(ResPath(UTF8ToUTF16("//model/teapot.obj")), desc);
  azer::MeshPtr obj2 = loader.Load(ResPath(UTF8ToUTF16("//model/trunk.obj")), desc);
  azer::MeshPtr obj3 = loader.Load(ResPath(UTF8ToUTF16("//model/venusm.obj")), desc);
  obj1->SetEffectAdapterContext(ctx->GetEffectAdapterContext());
  obj2->SetEffectAdapterContext(ctx->GetEffectAdapterContext());
  obj3->SetEffectAdapterContext(ctx->GetEffectAdapterContext());
  obj1->AddProvider(EffectParamsProviderPtr(p1));
  obj2->AddProvider(EffectParamsProviderPtr(p2));
  obj3->AddProvider(EffectParamsProviderPtr(p3));
  node1->mutable_data()->AttachMesh(obj1);
  node2->mutable_data()->AttachMesh(obj2);
  node3->mutable_data()->AttachMesh(obj3);
  node1->mutable_holder()->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  node2->mutable_holder()->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
  node3->mutable_holder()->SetPosition(Vector3(10.0f, 00.0f, 0.0f));
  node3->mutable_holder()->SetScale(Vector3(0.002f, 0.002f, 0.002f));
  scene_renderer_.reset(new SceneRender(scene_context_.get(), root_.get()));
}

bool RenderDelegate::Initialize() { 
  Vector3 camera_pos(0.0f, 1.0f, 5.0f);
  Vector3 lookat(0.0f, 1.0f, 0.0f);
  Vector3 up(0.0f, 1.0f, 0.0f);
  camera_.reset(camera_pos, lookat, up);
  camera_controller_.reset(new FPSCameraController(&camera_));
  view()->AddEventListener(camera_controller_.get());

  gridline_.reset(new CoordinateGrid(1.0f, 1.0f, 30));
  gridline_->SetXCoordColor(kGridLineColor);
  gridline_->SetZCoordColor(kGridLineColor);
  effect_ = CreateDiffuseEffect();
  InitScene();

  camera_overlay_.reset(new CameraOverlay(&camera_));
  this->window()->SetRenderUI(true);

  renderer_pane_ = new RendererInfoPane;
  renderer_pane_->SetBounds(10, 10, 180, 120);
  this->window()->AddChildView(renderer_pane_);
  // scene_renderer_->Update(args);
  return true;
}

void RenderDelegate::OnUpdate(const FrameArgs& args) {
  camera_controller_->Update(args);
  gridline_->Update(camera_);
  pv_ = camera_.GetProjViewMatrix();
  camera_overlay_->Update();
  Renderer* renderer = window()->GetRenderer().get();
  renderer_pane_->Update(renderer, args);
  scene_renderer_->Update(args);
}

void RenderDelegate::OnRender(const FrameArgs& args) {
  Renderer* renderer = window()->GetRenderer().get();
  renderer->Use();
  renderer->Clear(kRenderBgColor);
  renderer->ClearDepthAndStencil();
  renderer->SetCullingMode(kCullBack);
  renderer->EnableDepthTest(true);
  gridline_->Render(renderer);

  scene_renderer_->Draw(renderer, effect_.get(), azer::kTriangleList);
  camera_overlay_->Render(renderer);
}
}  // namespace lord
