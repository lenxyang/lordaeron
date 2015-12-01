#pragma once

#include "azer/render/render.h"
#include "azer/render/util.h"
#include "azer/render/geometry/normal_util.h"
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
  SceneNodePtr OnInitScene() override;
  void OnInitUI() override;
  void OnUpdateFrame(const FrameArgs& args) override;
  void OnRenderFrame(const FrameArgs& args, Renderer* renderer) override;
  bool CreateSinWave();
 private:
  DiffuseEffectPtr effect_;
  DirLight light_;
  Matrix4 pv_;
  Matrix4 world_;
  Vector4 color_;

  EntityPtr sinwave_;
  scoped_ptr<FPSCameraController> camera_controller_;
  DISALLOW_COPY_AND_ASSIGN(MyRenderWindow);
};

bool MyRenderWindow::CreateSinWave() {
  RenderSystem* rs = RenderSystem::Current();
  VertexDescPtr desc = effect_->GetVertexDesc();
  Tile tile(7);
  SlotVertexDataPtr vdata(new azer::SlotVertexData(
      desc, tile.GetVertexCount()));
  azer::VertexPack vpack(vdata.get());
  tile.GenerateVertex(10.0f, 10.0f, &vpack);

  float T = 1.0f;
  VertexPos pos_pos;
  CHECK(GetSemanticIndex("position", 0, desc, &pos_pos));
  vpack.first();
  while (!vpack.end()) {
    Vector3 v;
    vpack.ReadVector3Or4(&v, pos_pos);
    Vector4 vec = Vector4(v, 1.0f);
    float L = std::sqrt(vec.x * vec.x + vec.z * vec.z);
    vec.y = 0.1f * sin(15 * L * sin(T));
    vpack.next(1);
  }

  IndicesDataPtr idata(new IndicesData(tile.GetTriangleListIndexCount()));
  IndexPack ipack(idata.get());
  tile.GenerateTriangleListIndices(1, &ipack);
  CalcNormal(vdata.get(), idata.get());

  VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
  IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
  sinwave_ = new Entity(vb, ib);
  return true;
}

SceneNodePtr MyRenderWindow::OnInitScene() {
  effect_ = CreateDiffuseEffect();
  Context* ctx = Context::instance(); 
  light_.direction = Vector4(-0.6f, -0.2f, -0.2f, 0.0f);
  light_.diffuse = Vector4(0.8f, 0.8f, 1.8f, 1.0f);
  light_.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

  CreateSinWave();
  return SceneNodePtr();
}

void MyRenderWindow::OnInitUI() { 
  camera_controller_.reset(new FPSCameraController(mutable_camera()));
  view()->AddEventListener(camera_controller_.get());
}

void MyRenderWindow::OnUpdateFrame(const FrameArgs& args) {
  camera_controller_->Update(args);
  pv_ = camera().GetProjViewMatrix();
  world_ = Matrix4::kIdentity;
  color_ = Vector4(1.0f, 0.0f, 0.0f, 0.22f);
}

void MyRenderWindow::OnRenderFrame(const FrameArgs& args, Renderer* renderer) {
  lord::Context* ctx = lord::Context::instance();
  effect_->SetWorld(world_);
  effect_->SetPV(pv_);
  effect_->SetColor(color_);
  effect_->SetDirLight(light_);
  renderer->UseEffect(effect_.get());
  sinwave_->Render(renderer);
}
}  // namespace lord

int main(int argc, char* argv[]) {
  CHECK(lord::Context::InitContext(argc, argv));

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
