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
class RenderDelegate : public nelf::RenderDelegate,
                       public views::WidgetObserver {
 public:
  RenderDelegate();
  bool Init();
  virtual bool Initialize() override;
  virtual void OnUpdate(const azer::FrameArgs& args) override;
  virtual void OnRender(const azer::FrameArgs& args) override;

  // attributes
  const azer::Camera& camera() const { return camera_;}
  void InitScene();

  SceneNode* root() { return root_.get();}
  void OnWidgetBoundsChanged(views::Widget* widget, 
                             const gfx::Rect& new_bounds) override {
    gfx::Rect rect = view()->GetContentsBounds();
    float aspect = (float)rect.width() / (float)rect.height();
    camera_.frustrum().set_aspect(aspect);
  }

  void OnWidgetDestroying(views::Widget* widget) override {
    widget->RemoveObserver(this);
  }
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
  scoped_ptr<lord::RenderDelegate> delegate(new lord::RenderDelegate);
  lord::RenderDelegate* del = delegate.get();
  delegate->Init();
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

  std::string contents;
  base::ReadFileToString(base::FilePath(
      FILE_PATH_LITERAL("lordaeron/sandbox/scene/scene.sce")), &contents);
  ConfigNodePtr config_root = ConfigNode::InitFromXMLStr(contents);
  CHECK(config_root.get());

  VertexDescPtr desc = effect_->GetVertexDesc();
  MySceneLoaderDelegate delegate(fsystem_.get(), desc);
  SceneLoader loader(&delegate);
  CHECK(loader.Load(root_.get(), config_root));
  scene_renderer_.reset(new SceneRender(scene_context_.get(), root_.get()));
}

bool RenderDelegate::Initialize() { 
  camera_overlay_.reset(new CameraOverlay(&camera_));
  this->window()->SetRenderUI(true);

  renderer_pane_ = new RendererInfoPane;
  renderer_pane_->SetBounds(10, 10, 180, 120);
  this->window()->AddChildView(renderer_pane_);
  // scene_renderer_->Update(args);

  Vector3 camera_pos(0.0f, 1.0f, 5.0f);
  Vector3 lookat(0.0f, 1.0f, 0.0f);
  Vector3 up(0.0f, 1.0f, 0.0f);
  camera_.reset(camera_pos, lookat, up);
  camera_controller_.reset(new FPSCameraController(&camera_));
  view()->AddEventListener(camera_controller_.get());

  lord::SceneTreeWindow* scene = new lord::SceneTreeWindow(
      gfx::Rect(400, 300), this->window()->GetTopWindow());
  scene->SetSceneNode(root_);
  scene->Init();
  scene->Show();
  scene->SetTitle(base::UTF8ToUTF16("Scene"));

  window()->GetWidget()->AddObserver(this);
  return true;
}

bool RenderDelegate::Init() { 
  gridline_.reset(new CoordinateGrid(1.0f, 1.0f, 30));
  gridline_->SetXCoordColor(kGridLineColor);
  gridline_->SetZCoordColor(kGridLineColor);
  effect_ = CreateDiffuseEffect();
  InitScene();
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
