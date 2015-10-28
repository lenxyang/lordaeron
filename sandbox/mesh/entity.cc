#pragma once

#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/ui/scene_render_window.h"

namespace lord {

using namespace azer;
class MyRenderWindow : public SceneRenderWindow {
 public:
  MyRenderWindow(const gfx::Rect& rect) : SceneRenderWindow(rect) {}
  void OnInitScene() override;
  void OnInitUI() override;
  void OnUpdateFrame(const FrameArgs& args) override;
  void OnRenderFrame(const FrameArgs& args, Renderer* renderer) override;

  SceneNode* root() { return root_.get();}
 private:
  DiffuseEffectPtr effect_;
  DirLight light_;
  Matrix4 pv_;
  Matrix4 world_;

  MeshPtr mesh_;
  scoped_ptr<FPSCameraController> camera_controller_;
  scoped_ptr<FileSystem> fsystem_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};

void MyRenderWindow::OnInitScene() {
  effect_ = CreateDiffuseEffect();
  Context* ctx = Context::instance(); 
  fsystem_.reset(new NativeFileSystem(
      ::base::FilePath(FILE_PATH_LITERAL("lordaeron/media"))));
  light_.dir = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  light_.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  light_.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

  ModelLoader loader(fsystem_.get());
  VertexDescPtr desc = effect_->GetVertexDesc();
  mesh_ = loader.Load(ResPath(UTF8ToUTF16("//model/teapot.obj")), desc);
}

void MyRenderWindow::OnInitUI() { 
  camera_controller_.reset(new FPSCameraController(mutable_camera()));
  view()->AddEventListener(camera_controller_.get());
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  camera_controller_->Update(args);
  pv_ = camera().GetProjViewMatrix();
  mesh_->UpdateProviderParams(args);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, 
                                   Renderer* renderer) {
  mesh_->Render(renderer);
}
}  // namespace lord
