#include "lordaeron/scene/scene_bounding_volumn.h"

#include "base/logging.h"
#include "base/lazy_instance.h"
#include "azer/render/render.h"
#include "azer/render/util.h"

#include "lordaeron/context.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/effect/diffuse_effect.h"

namespace lord {
using namespace azer;

namespace {
::base::LazyInstance<EffectAdapterContext> adapter_context_
= LAZY_INSTANCE_INITIALIZER;
}

SceneBVRenderNode::SceneBVRenderNode(SceneNode* node)
    : SceneRenderNode(node),
      color_(Vector4(1.0f, 0.0f, 0.0f, 0.3f)) {
  if (!adapter_context_.Pointer()->LookupAdapter(
          SceneBVParamsAdapter::kAdapterKey)) {
    adapter_context_.Pointer()->RegisteAdapter(new SceneBVParamsAdapter);
  }
}

SceneBVRenderNode::~SceneBVRenderNode() {
}

void SceneBVRenderNode::Init() {
  MeshPtr mesh = CreateBoundingBoxForSceneNode(node_);
  mesh->AddProvider(this);
  AddMesh(mesh);
}

void SceneBVRenderNode::Update(const azer::FrameArgs& args) {
  Vector3 vmin = node_->vmin();
  Vector3 vmax = node_->vmax();
  Vector3 center = (vmin + vmax) * 0.5f;
  Vector3 scale(vmax.x - vmin.x, vmax.y - vmin.y, vmax.z - vmin.z);
  world_ = std::move(Scale(scale));
  world_ = std::move(node_->orientation().ToMatrix()) * world_;
  world_ = std::move(Translate(center)) * world_;
  if (parent()) {
    world_ = std::move(parent()->GetWorld() * world_);
  }

  if (node_->is_draw_bounding_volumn()) {
    CHECK(mesh_.get());
    mesh_->UpdateProviderParams(args);
  }
}

void SceneBVRenderNode::Render(azer::Renderer* renderer) {
  if (node_->is_draw_bounding_volumn()) {
    mesh_->Render(renderer);
  }
}

// class SceneBVParamsAdapter
const EffectAdapterKey SceneBVParamsAdapter::kAdapterKey =
    std::make_pair(typeid(DiffuseEffect).name(),
                   typeid(SceneBVRenderNode).name());

EffectAdapterKey SceneBVParamsAdapter::key() const { return kAdapterKey; }

void SceneBVParamsAdapter::Apply(
    Effect* e, const EffectParamsProvider* params) const {
  Context* ctx = Context::instance();
  const SceneBVRenderNode* node = 
      dynamic_cast<const SceneBVRenderNode*>(params);
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(node && effect);

  effect->SetWorld(node->GetWorld());
  effect->SetColor(node->color());
  effect->SetPV(node->camera()->GetProjViewMatrix());
  effect->SetDirLight(ctx->GetInternalLight());
}

MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node) {
  Context* ctx = Context::instance();
  EffectPtr effect = ctx->GetEffect(DiffuseEffect::kEffectName);
  BoxObject* objptr = new BoxObject(effect->GetVertexDesc());
  
  MeshPartPtr objpart = objptr->CreateObject(effect.get());
  BlendingPtr blending = ctx->GetDefaultBlending();
  objpart->SetBlending(blending);
  MeshPartPtr framepart = objptr->CreateFrameObject(effect.get());
  MeshPtr mesh(new Mesh(adapter_context_.Pointer()));
  mesh->AddMeshPart(framepart);
  return mesh;
}
}  // namespace lord
