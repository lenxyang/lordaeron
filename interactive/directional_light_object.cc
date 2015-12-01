#include "lordaeron/interactive/directional_light_object.h"

#include "base/lazy_instance.h"
#include "base/logging.h"
#include "lordaeron/context.h"
#include "lordaeron/interactive/axis_object.h"

namespace lord {
DirectionalLightObject::DirectionalLightObject(LightPtr light)
    : light_(light) {
  CHECK_EQ(light->type(), kDirectionalLight);
  effect_ = CreateDiffuseEffect();
  axis_.reset(new AxisObject(effect_->GetVertexDesc()));
}

DirectionalLightObject::~DirectionalLightObject() {
}

void DirectionalLightObject::Render(const azer::Matrix4& pv, 
                                    azer::Renderer* renderer) {
  Context* context = Context::instance();
  effect_->SetDirLight(context->GetInternalLight());
  effect_->SetColor(light_->diffuse());
  effect_->SetPV(pv);
  effect_->SetWorld(azer::Matrix4::kIdentity);
  renderer->UseEffect(effect_.get());
  axis_->Render(renderer);
}

}  // namespace lord
