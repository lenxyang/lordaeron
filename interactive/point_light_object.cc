#include "lordaeron/interactive/point_light_object.h"

#include "base/lazy_instance.h"
#include "lordaeron/context.h"

namespace lord {

namespace {
::base::LazyInstance<PointLightObject> point_light_object = LAZY_INSTANCE_INITIALIZER;
}  // namespace

using namespace azer;
PointLightObject::PointLightObject() {
  range_color_ = Vector4(1.0f, 1.0f, 1.0f, 0.1f);
  color_ = Vector4(2.0f, 2.0f, 2.0f, 1.0f);
  effect_ = CreateDiffuseEffect();
  sphere_ = 
}

PointLightObject::~PointLightObject() {
}

void PointLightObject::SetRange(float range) {
  scale_ = range;
}

void PointLightObject::Render(const azer::Matrix4& pv, azer::Renderer* renderer) {
  Context* context = Context::instance();
  Matrix4 world = Translate(position_);
  Matrix4 range_scale = Scale(scale_, scale_, scale_);

  effect_->SetDirLight(context->GetInternalLight());
  effect_->SetPV(pv);
  effect_->SetWorld(world);
  effect_->SetColor(color_);
  renderer->UseEffect(effect_.get());
  sphere_->Draw(renderer);

  BlendingPtr blending = context->GetDefaultBlending();
  renderer->UseBlending(blending.get(), 0);
  effect_->SetWorld(std::move(world * range_scale_));
  effect_->SetColor(range_color_);
  renderer->UseEffect(effect_.get());
  sphere_->Draw(renderer);
  renderer->ResetBlending();
}

PointLightObject* GetPointLightObjectPointer() {
  return point_light_object.Pointer();
}
}  // namespace lord
