#pragma once

#include "lordaeron/sandbox/sandbox.h"

namespace lord {

using namespace azer;

class MyProvider : public azer::EffectParamsProvider {
 public:
  MyProvider(const Camera& camera) : camera_(camera) {
    light_.dir = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
    light_.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
    light_.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
  }

  void UpdateParams(const FrameArgs& args) override {
    pv_ = camera_.GetProjViewMatrix();
    world_ = Matrix4::kIdentity;
    color_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
  }
 private:
  friend class MyProviderAdapter;
  Matrix4 pv_;
  Matrix4 world_;
  Vector4 color_;
  DirLight light_;
  const Camera& camera_;
};

class MyProviderAdapter : public azer::EffectParamsAdapter {
 public:
  EffectAdapterKey key() const override {
    return std::make_pair(typeid(DiffuseEffect).name(), typeid(MyProvider).name());
  }
  void Apply(Effect* e, const EffectParamsProvider* params) const override {
    CHECK(typeid(*e) == typeid(DiffuseEffect));
    CHECK(typeid(*params) == typeid(MyProvider));
    MyProvider* provider = (MyProvider*)params;
    DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
    effect->SetWorld(provider->world_);
    effect->SetPV(provider->pv_);
    effect->SetColor(provider->color_);
    effect->SetDirLight(provider->light_);
  }
};

class MyRenderWindow : public SimpleRenderWindow {
 public:
  MyRenderWindow(const gfx::Rect& rect) : SimpleRenderWindow(rect) {}
  void OnInitScene() override;
  void OnInitUI() override;
  void OnUpdateFrame(const FrameArgs& args) override;
  void OnRenderFrame(const FrameArgs& args, Renderer* renderer) override;
 private:
  DiffuseEffectPtr effect_;
  MeshPtr mesh_;
  scoped_ptr<FPSCameraController> camera_controller_;
  EffectAdapterContext context_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};

void MyRenderWindow::OnInitScene() {
  effect_ = CreateDiffuseEffect();
  context_.RegisteAdapter(new MyProviderAdapter);
  Context* ctx = Context::instance(); 
  scoped_ptr<FileSystem> fsystem;
  fsystem.reset(new azer::NativeFileSystem(
      ::base::FilePath(FILE_PATH_LITERAL("lordaeron/media"))));
  ModelLoader loader(fsystem.get());
  std::string pathstr = "//model/cow.obj";
  mesh_ = loader.Load(ResPath(::base::UTF8ToUTF16(pathstr)), 
                      effect_->GetVertexDesc());
  EffectParamsProviderPtr provider = new MyProvider(camera());
  mesh_->AddProvider(provider);
  mesh_->SetEffectAdapterContext(&context_);
  for (int32 i = 0; i < mesh_->part_count(); ++i) {
    mesh_->part_at(i)->SetEffect(effect_.get());
  }
}

void MyRenderWindow::OnInitUI() { 
  camera_controller_.reset(new FPSCameraController(mutable_camera()));
  view()->AddEventListener(camera_controller_.get());
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  camera_controller_->Update(args);
  mesh_->UpdateProviderParams(args);
  
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  mesh_->Render(renderer);
}
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

  window->GetRenderLoop()->Run();
  return 0;
}
