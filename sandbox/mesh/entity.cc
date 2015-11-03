#pragma once

#include "lordaeron/sandbox/sandbox.h"

namespace lord {

using namespace azer;
class MyRenderWindow : public SimpleRenderWindow {
 public:
  MyRenderWindow(const gfx::Rect& rect) : SimpleRenderWindow(rect) {}
  void OnInitScene() override;
  void OnInitUI() override;
  void OnUpdateFrame(const FrameArgs& args) override;
  void OnRenderFrame(const FrameArgs& args, Renderer* renderer) override;
 private:
  DiffuseEffectPtr effect_;
  DirLight light_;
  Matrix4 pv_;
  Matrix4 world_;
  Vector4 color_;

  EntityVecPtr entity_;
  scoped_ptr<FPSCameraController> camera_controller_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};

void MyRenderWindow::OnInitScene() {
  effect_ = CreateDiffuseEffect();
  Context* ctx = Context::instance(); 
  light_.dir = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  light_.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  light_.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

  scoped_ptr<FileSystem> fsystem;
  fsystem.reset(new azer::NativeFileSystem(
      ::base::FilePath(FILE_PATH_LITERAL("lordaeron/media"))));
  ModelLoader loader(fsystem.get());
  std::string pathstr = "//model/cow.obj";
  entity_ = loader.LoadVertexData(ResPath(::base::UTF8ToUTF16(pathstr)), 
                                  effect_->GetVertexDesc());
}

void MyRenderWindow::OnInitUI() { 
  camera_controller_.reset(new FPSCameraController(mutable_camera()));
  view()->AddEventListener(camera_controller_.get());
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  camera_controller_->Update(args);
  pv_ = camera().GetProjViewMatrix();
  world_ = Matrix4::kIdentity;
  color_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  effect_->SetWorld(world_);
  effect_->SetPV(pv_);
  effect_->SetColor(color_);
  effect_->SetDirLight(light_);
  renderer->UseEffect(effect_.get());
  for (int32 i = 0; i < entity_->entity_count(); ++i) {
    entity_->entity_at(i)->Render(renderer);
  }
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
