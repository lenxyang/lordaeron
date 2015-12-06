#include "lordaeron/interactive/light_controller.h"

#include "base/logging.h"
#include "lordaeron/context.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using namespace azer;
PointLightObject::PointLightObject(const azer::Camera* camera, SceneNode* node) 
    : camera_(camera),
      node_(node) {
  DCHECK(node->type() == kLampSceneNode);
  effect_ = CreateDiffuseEffect();
  Light* light = node->mutable_data()->light();
  float range = light->point_light().atten.range;
  Context* ctx = Context::instance();
  BlendingPtr blending = ctx->GetDefaultBlending();
  GeometryObjectPtr obj = new SphereObject(effect_->GetVertexDesc(), range);
  sphere_ = obj->CreateObject(effect_.get());
  sphere_->SetBlending(blending.get());
}

PointLightObject::~PointLightObject() {
}

void PointLightObject::Update(const azer::FrameArgs& args) {
  world_ = Translate(node_->GetWorldPosition());
  pv_ = camera_->GetProjViewMatrix();
}

void PointLightObject::Render(azer::Renderer* renderer) {
  Context* ctx = Context::instance();
  Light* light = node_->mutable_data()->light();
  effect_->SetDirLight(ctx->GetInternalLight());
  Vector4 color = light->point_light().diffuse;
  color.w = 0.2f;
  effect_->SetColor(color);
  effect_->SetWorld(world_);
  effect_->SetPV(pv_);
  renderer->UseEffect(effect_.get());
  sphere_->Render(renderer);
}
}  // namespace lord
