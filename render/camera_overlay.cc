#include "lordaeron/render/camera_overlay.h"

#include "azer/math/math.h"
#include "azer/render/render.h"
#include "lordaeron/env.h"
#include "lordaeron/render/coordinate.h"

namespace lord {
using namespace azer;
CameraOverlay::CameraOverlay(const Camera* camera) 
    : camera_(camera) {
  RenderSystem* rs = RenderSystem::Current();
    azer::Texture::Options rdopt;
  rdopt.size = gfx::Size(128, 128);
  rdopt.target = (kBindTargetRenderTarget | kBindTargetShaderResource);
  renderer_ = rs->CreateRenderer(rdopt);
  viewport_.bounds = gfx::Rect(rdopt.size);
  renderer_->SetViewport(viewport_);

  overlay_ = rs->CreateOverlay();
  overlay_->SetBounds(gfx::RectF(0.75f, 0.75f, 0.25f, 0.25f));
  overlay_->SetTexCoord(gfx::PointF(0.0f, 0.0f), gfx::PointF(1.0f, 1.0f));
  object_.reset(new AxesFrame);
}

CameraOverlay::~CameraOverlay() {}

void CameraOverlay::SetShowBounds(const gfx::RectF& bounds) {
  overlay_->SetBounds(bounds);
}

void CameraOverlay::Update() {
  const TransformHolder& holder = camera_->holder();
  
  Vector3 position(-holder.direction() * 2.0f);
  Vector3 lookat(0.0f, 0.0f, 0.0f);
  Vector3 up(holder.up());
  Camera camera;
  camera.reset(position, lookat, up);
  camera.mutable_frustum()->set_aspect(1.0f);
  world_ = Matrix4::kIdentity;

  Renderer* texrd = renderer_.get();
  texrd->Use();
  texrd->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 0.0f));
  texrd->ClearDepthAndStencil();
  object_->SetWorld(world_);
  object_->SetProjView(camera.GetProjViewMatrix());
  object_->Render(texrd);
}

void CameraOverlay::Render(Renderer* renderer) {
  LordEnv* context = LordEnv::instance();
  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  overlay_->SetTexture(renderer_->GetRenderTarget(0)->GetTexture());
  overlay_->Render(renderer);
  renderer->ResetBlending();
}
}  // namespace lord
