#include "lordaeron/scene/ui_scene_render.h"

#include "azer/render/render.h"
#include "lordaeron/env.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/effect/normal_line_effect.h"
#include "lordaeron/interactive/light_controller.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {
using namespace azer;
const azer::EffectAdapterKey LordSceneBVParamsAdapter::kAdapterKey =
    std::make_pair(typeid(DiffuseEffect).name(),
                   typeid(LordSceneBVRenderProvider).name());

azer::EffectAdapterKey LordSceneBVParamsAdapter::key() const {
  return kAdapterKey;
}
void LordSceneBVParamsAdapter::Apply(Effect* e,const EffectParamsProvider* p) const {
  LordEnv* ctx = LordEnv::instance();
  const LordSceneBVRenderProvider* provider =
      dynamic_cast<const LordSceneBVRenderProvider*>(p);
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(provider && effect);
  effect->SetWorld(provider->GetWorld());
  effect->SetColor(provider->color());
  effect->SetPV(provider->GetPV());
  effect->SetDirLight(ctx->GetInternalLight());
}
LordSceneBVRenderProvider::LordSceneBVRenderProvider(SceneRenderNode* node)
    : color_(Vector4(1.0f, 0.0f, 0.0f, 0.3f)), node_(node) {}

void LordSceneBVRenderProvider::UpdateParams(const azer::FrameArgs& args) {
  SceneNode* snode = node_->GetSceneNode();
  Vector3 vmin = snode->vmin();
  Vector3 vmax = snode->vmax();
  Vector3 center = (vmin + vmax) * 0.5f;
  Vector3 scale(vmax.x - vmin.x, vmax.y - vmin.y, vmax.z - vmin.z);
  scale_ = std::move(Scale(scale));
  world_ = std::move(node_->GetWorld() * Translate(center)
                     * scale_);
}

const azer::Matrix4& LordSceneBVRenderProvider::GetPV() const {
  return node_->GetPV();
}

// class LordObjectNodeRenderDelegate
LordObjectNodeRenderDelegate::LordObjectNodeRenderDelegate(
    SceneRenderNode* node, UISceneRenderer* renderer)
    : SceneRenderNodeDelegate(node),
      tree_renderer_(renderer) {
  Init();
}

bool LordObjectNodeRenderDelegate::Init() {
  SceneNode* scene_node = GetSceneNode();
  CHECK(scene_node->type() == kObjectSceneNode
        || scene_node->type() == kSceneNode);

  if (scene_node->type() == kObjectSceneNode) {
    mesh_ = scene_node->mutable_data()->GetMesh();
    mesh_->AddProvider(node_);
    if (node_->GetEnvNode())
      mesh_->AddProvider(node_->GetEnvNode());
  }

  bounding_mesh_ = CreateBoundingBoxForSceneNode(scene_node);
  EffectParamsProviderPtr provider(new LordSceneBVRenderProvider(node_));
  bounding_mesh_->AddProvider(provider);
  normal_mesh_ = CreateNormalLineMeshForSceneNode(scene_node);
  if (normal_mesh_.get())
    normal_mesh_->AddProvider(node_);
  return true;
}

void LordObjectNodeRenderDelegate::Update(const FrameArgs& args) {
  if (mesh_.get()) {
    mesh_->UpdateProviderParams(args);
  }
  if (normal_mesh_.get())
    normal_mesh_->UpdateProviderParams(args);
  bounding_mesh_->UpdateProviderParams(args);
}

void LordObjectNodeRenderDelegate::Render(Renderer* renderer) {
  if (mesh_.get())
    mesh_->Render(renderer);

  SceneNode* scene_node = GetSceneNode();
  if (scene_node->is_draw_bounding_volumn()) {
    tree_renderer_->AddBoundingVolumnMesh(bounding_mesh_);
    
    if (normal_mesh_.get())
      normal_mesh_->Render(renderer);
  }
}

// class LordLampNodeRenderDelegate
LordLampNodeRenderDelegate::LordLampNodeRenderDelegate(SceneRenderNode* node)
    : SceneRenderNodeDelegate(node),
      controller_(NULL) {
  SceneNode* scene_node = GetSceneNode();
  CHECK(scene_node->type() == kLampSceneNode);
  CHECK(scene_node->parent() && scene_node->parent()->type() == kEnvSceneNode);
  Init();
}

bool LordLampNodeRenderDelegate::Init() {
  SceneNode* scene_node = GetSceneNode();
  Light* light = scene_node->mutable_data()->light();
  switch (light->type()) {
    case kDirectionalLight:
      controller_ = new DirLightController(node_);
      break;
    case kSpotLight:
      controller_ = new SpotLightController(node_);
      break;
    case kPointLight:
      controller_ = new PointLightController(node_);
      break;
    default:
      CHECK(false);
  }

  scene_node->SetMin(controller_->GetLightMesh()->vmin());
  scene_node->SetMax(controller_->GetLightMesh()->vmax());
  return true;
}

void LordLampNodeRenderDelegate::Update(const FrameArgs& args) {
  controller_->Update(args);
}

void LordLampNodeRenderDelegate::Render(Renderer* renderer) {
  controller_->Render(renderer);
}

namespace {
class SceneNodeDelegateFactory : public SceneNodeRenderDelegateFactory {
 public:
  SceneNodeDelegateFactory(UISceneRenderer* renderer)
      : tree_renderer_(renderer) {}
  scoped_ptr<SceneRenderNodeDelegate> CreateDelegate(SceneRenderNode* node) override;
 private:
  UISceneRenderer* tree_renderer_;
};
scoped_ptr<SceneRenderNodeDelegate>
SceneNodeDelegateFactory::CreateDelegate(SceneRenderNode* node) {
  switch (node->GetSceneNode()->type()) {
    case kEnvSceneNode:
      return NULL;
    case kSceneNode:
    case kObjectSceneNode:
      return scoped_ptr<SceneRenderNodeDelegate>(
          new LordObjectNodeRenderDelegate(node, tree_renderer_)).Pass();
    case kLampSceneNode:
      return scoped_ptr<SceneRenderNodeDelegate>(
          new LordLampNodeRenderDelegate(node)).Pass();
    default:
      NOTREACHED() << "no such type supported: " << node->GetSceneNode()->type();
      return scoped_ptr<SceneRenderNodeDelegate>().Pass();
  }
}
}

// class UISceneRenderer
UISceneRenderer::UISceneRenderer() {
  scoped_ptr<SceneNodeDelegateFactory> factory(new SceneNodeDelegateFactory(this));
  SetDelegateFactory(factory.Pass());
}

void UISceneRenderer::OnFrameUpdateBegin(const azer::FrameArgs& args) {
  blending_node_.clear();
  bvmesh_.clear();
  normal_mesh_.clear();
}

void UISceneRenderer::OnFrameRenderBegin(azer::Renderer* renderer) {
}

void UISceneRenderer::OnFrameUpdateEnd(const azer::FrameArgs& args) {
}

void UISceneRenderer::OnFrameRenderEnd(azer::Renderer* renderer) {
  {
    ScopedDepthBuffer(false, renderer);
    for (auto iter = blending_node_.begin(); iter != blending_node_.end(); ++iter) {
      (*iter)->Render(renderer);
    }
  }

  for (auto iter = blending_node_.begin(); iter != blending_node_.end(); ++iter) {
    (*iter)->Render(renderer);
  }

  for (auto iter = bvmesh_.begin(); iter != bvmesh_.end(); ++iter) {
    (*iter)->Render(renderer);
  }
}

bool UISceneRenderer::UpdateNode(SceneRenderNode* node, const FrameArgs& args) {
  node->Update(args);
  return true;
}

bool UISceneRenderer::RenderNode(SceneRenderNode* node, Renderer* renderer) {
  if (!node->GetSceneNode()->visible()) {
    return false;
  }

  if (node->GetSceneNode()->type() != kLampSceneNode) {
    node->Render(renderer);
  } else {
    blending_node_.push_back(node);
  }
  for (auto iter = node->children().begin(); 
       iter != node->children().end(); ++iter) {
    RenderNode(iter->get(), renderer);
  }

  return true;
}

MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node) {
  LordEnv* ctx = LordEnv::instance();
  EffectPtr effect = ctx->GetEffect(DiffuseEffect::kEffectName);
  BoxObject* objptr = new BoxObject(effect->vertex_desc());
  
  MeshPartPtr objpart = objptr->CreateObject(effect.get());
  BlendingPtr blending = ctx->GetDefaultBlending();
  objpart->SetBlending(blending);
  MeshPartPtr framepart = objptr->CreateFrameObject(effect.get());
  MeshPtr mesh(new Mesh(ctx->GetEffectAdapterContext()));
  mesh->AddMeshPart(framepart);
  mesh->AddMeshPart(objpart);
  return mesh;
}

MeshPtr CreateNormalLineMeshForSceneNode(SceneNode* node) {
  if (node->type() != kObjectSceneNode
      || node->mutable_data()->GetMesh() == NULL) {
    return MeshPtr();
  }

  LordEnv* ctx = LordEnv::instance();
  EffectPtr effect = ctx->GetEffect(NormalLineEffect::kEffectName);
  MeshPtr object_mesh = node->mutable_data()->GetMesh();
  MeshPtr normal_mesh(new Mesh(ctx->GetEffectAdapterContext()));
  for (int32 i = 0; i < object_mesh->part_count(); ++i) {
    MeshPart* orgpart = object_mesh->part_at(i);
    MeshPartPtr part(new MeshPart(effect.get(), orgpart->entity_vector()));
    normal_mesh->AddMeshPart(part);
  }
  
  return normal_mesh;
}
}  // namespace lord
