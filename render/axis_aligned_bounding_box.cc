#include "lordaeron/render/axis_aligned_bounding_box.h"

#include "base/logging.h"
#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/context.h"

namespace lord {
using namespace azer;
AxisAlignedBoundingBox::AxisAlignedBoundingBox(Vector3 vmin, Vector3 vmax) 
    : BoundingVolumn(kAxisAlignedBoundingBox), 
      vmin_(vmin), 
      vmax_(vmax),
      color_(Vector4(1.0f, 0.0f, 0.0f, 1.0f)) {
  effect_ = CreateColoredDiffuseEffect();
  object_ = new BoxObject(effect_->GetVertexDesc());
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox()
    : BoundingVolumn(kAxisAlignedBoundingBox), 
      vmin_(Vector3( 99999.0f,  99999.0f,  99999.0f)),
      vmax_(Vector3(-99999.0f, -99999.0f, -99999.0f)),
      color_(Vector4(1.0f, 0.0f, 0.0f, 1.0f)) {
  effect_ = CreateColoredDiffuseEffect();
  object_ = new BoxObject(effect_->GetVertexDesc());
}

void AxisAlignedBoundingBox::Update(const Camera& camera, 
                                    const TransformHolder& holder) {
  Vector3 scale(vmax_.x - vmin_.x, vmax_.y - vmin_.y, vmax_.z - vmin_.z);
  Vector3 pos((vmax_.x - vmin_.x) * 0.5f, 
                    (vmax_.y - vmin_.y) * 0.5f, 
                    (vmax_.z - vmin_.z) * 0.5f);
  Vector3 nscale = holder.scale();
  Vector3 npos = holder.position();
  world_ = std::move(Scale(Vector3(scale.x * nscale.x,
                                   scale.y * nscale.y,
                                   scale.z * nscale.z)));
  Vector3 curpos = npos + pos;
  world_ = std::move(Translate(curpos) * world_);
  pvw_ = camera.GetProjViewMatrix();
}

void AxisAlignedBoundingBox::Render(Renderer* renderer) {
  Context* context = Context::instance();
  ColoredDiffuseEffect* effect = dynamic_cast<ColoredDiffuseEffect*>(effect_.get());
  DCHECK(effect);
  effect->SetWorld(world_);
  effect->SetPVW(pvw_);
  effect->SetColor(color_);
  effect->Use(renderer);
  object_->RenderWireframe(renderer);

  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  object_->RenderWireframe(renderer);
  renderer->ResetBlending();
}

}  // namespace lord
