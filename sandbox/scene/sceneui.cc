#include <memory>

#include "base/strings/utf_string_conversions.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "azer/files/native_file_system.h"
#include "azer/render/util.h"
#include "ui/views/widget/widget_observer.h"
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
#include "lordaeron/scene/scene_loader.h"
#include "lordaeron/ui/scene_tree_view.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/ui/scene_render_window.h"
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

class MySceneLoaderDelegate : public lord::SceneLoaderDelegate {
 public:
  MySceneLoaderDelegate(FileSystem* fs, VertexDescPtr desc)
      : fsystem_(fs), desc_(desc) {}
  bool InitSceneNode(SceneNode* node, azer::ConfigNode* config) override {
    lord::Context* ctx = lord::Context::instance(); 
    const std::string& type =  config->GetAttr("type");
    if (type == "mesh") {
      DCHECK(config->HasNamedChild("mesh"));
      ConfigNode* mesh_node = config->GetNamedChildren("mesh")[0];
      DCHECK(mesh_node->HasNamedChild("provider"));
      ConfigNode* provider_node = mesh_node->GetNamedChildren("provider")[0];
      azer::MeshPtr mesh = LoadMesh(mesh_node);
      mesh->SetEffectAdapterContext(ctx->GetEffectAdapterContext());
      mesh->AddProvider(LoadProvider(provider_node));
      node->mutable_data()->AttachMesh(mesh);
      return true;
    } else {
      return true;
    }
  }
  
  azer::MeshPtr LoadMesh(azer::ConfigNode* config) {
    std::string pathstr;
    if (!config->GetChildText("path", &pathstr)) {
      return azer::MeshPtr();
    }
      
    lord::ModelLoader loader(fsystem_);
    azer::MeshPtr obj = loader.Load(ResPath(UTF8ToUTF16(pathstr)), desc_);
    return obj;
  }
  
  azer::EffectParamsProviderPtr LoadProvider(azer::ConfigNode* config) {
    lord::DiffuseEffectProvider* p(new lord::DiffuseEffectProvider);
    p->SetColor(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
    return azer::EffectParamsProviderPtr(p);
  }
 private:
  FileSystem* fsystem_;
  VertexDescPtr desc_;
};

namespace lord {
class RendererInfoPane;
class MyRenderWindow : public lord::SceneRenderWindow {
 public:
  MyRenderWindow(const gfx::Rect& rect) : lord::SceneRenderWindow(rect) {}
  void OnInitScene() override;
  void OnInitUI() override;
  void OnUpdateFrame(const azer::FrameArgs& args) override;
  void OnRenderFrame(const azer::FrameArgs& args, Renderer* renderer) override;


  SceneNode* root() { return root_.get();}
 private:
  SceneNodePtr root_;
  SceneContextPtr scene_context_;
  scoped_ptr<SceneRender> scene_renderer_;
  lord::DiffuseEffectPtr effect_;
  azer::Matrix4 pv_;

  scoped_ptr<azer::FPSCameraController> camera_controller_;
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

  lord::ObjectControlToolbar* toolbar = new lord::ObjectControlToolbar(window);
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
  dirlight.dir = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  dirlight.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  dirlight.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
  LightPtr light(new Light(dirlight));
  scene_context_ = new lord::SceneContext;
  scene_context_->GetGlobalEnvironment()->SetCamera(mutable_camera());
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

void MyRenderWindow::OnInitUI() { 
  camera_controller_.reset(new FPSCameraController(mutable_camera()));
  view()->AddEventListener(camera_controller_.get());

  lord::SceneTreeWindow* scene = new lord::SceneTreeWindow(
      gfx::Rect(400, 300), this->window()->GetTopWindow());
  scene->SetSceneNode(root_);
  scene->Init();
  scene->Show();
  scene->SetTitle(base::UTF8ToUTF16("Scene"));
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  camera_controller_->Update(args);
  pv_ = camera().GetProjViewMatrix();
  Renderer* renderer = window()->GetRenderer().get();
  scene_renderer_->Update(args);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  scene_renderer_->Draw(renderer, effect_.get(), azer::kTriangleList);
}
}  // namespace lord
