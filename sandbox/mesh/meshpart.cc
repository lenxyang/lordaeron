#pragma once

#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/res/grit/common.h"

namespace lord {

using namespace azer;
class MyRenderWindow : public SceneRenderWindow {
 public:
  MyRenderWindow(const gfx::Rect& rect) : SceneRenderWindow(rect) {}
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

  EntityPtr entity_;
  scoped_ptr<FPSCameraController> camera_controller_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};

void MyRenderWindow::OnInitScene() {
  effect_ = CreateDiffuseEffect();
  Context* ctx = Context::instance(); 
  light_.dir = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  light_.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  light_.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

  VertexDescPtr desc = effect_->GetVertexDesc();
  GeometryObjectPtr objptr = new SphereObject(effect_->GetVertexDesc(), 32, 32);
  entity_ = new Entity(objptr->GetVertexBuffer(), objptr->GetIndicesBuffer());
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
  effect_->Use(renderer);
  entity_->DrawIndex(renderer);
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

  lord::ObjectControlToolbar* toolbar = new lord::ObjectControlToolbar(window);
  window->GetRenderLoop()->Run();
  return 0;
}
