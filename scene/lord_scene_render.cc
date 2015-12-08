#include "lordaeron/scene/lord_scene_render.h"

#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/light_controller.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {
using namespace azer;
const azer::EffectAdapterKey LoadSceneBVParamsAdapter::kAdapterKey =
    std::make_pair(typeid(DiffuseEffect).name(),
                   typeid(LoadSceneBVRenderProvider).name());

azer::EffectAdapterKey LoadSceneBVParamsAdapter::key() const {
  return kAdapterKey;
}
void LoadSceneBVParamsAdapter::Apply(Effect* e,const EffectParamsProvider* p) const {
  Context* ctx = Context::instance();
  const LoadSceneBVRenderProvider* provider =
      dynamic_cast<const LoadSceneBVRenderProvider*>(p);
  DiffuseEffect* effect = dynamic_cast<DiffuseEffect*>(e);
  DCHECK(provider && effect);
  effect->SetWorld(provider->GetWorld());
  effect->SetColor(provider->color());
  effect->SetPV(provider->GetPV());
  effect->SetDirLight(ctx->GetInternalLight());
}
LoadSceneBVRenderProvider::LoadSceneBVRenderProvider(SceneRenderNode* node)
    : color_(Vector4(1.0f, 0.0f, 0.0f, 0.3f)), node_(node) {}

void LoadSceneBVRenderProvider::UpdateParams(const azer::FrameArgs& args) {
  SceneNode* snode = node_->GetSceneNode();
  Vector3 vmin = snode->vmin();
  Vector3 vmax = snode->vmax();
  Vector3 center = (vmin + vmax) * 0.5f;
  Vector3 scale(vmax.x - vmin.x, vmax.y - vmin.y, vmax.z - vmin.z);
  scale_ = std::move(Scale(scale));
  world_ = node_->GetWorld() * scale_;
}

const azer::Matrix4& LoadSceneBVRenderProvider::GetPV() const {
  return node_->GetPV();
}

// class LordObjectNodeRenderDelegate
LordObjectNodeRenderDelegate::LordObjectNodeRenderDelegate(SceneRenderNode* node)
    : SceneRenderNodeDelegate(node) {
}

bool LordObjectNodeRenderDelegate::Init() {
  SceneNode* scene_node = GetSceneNode();
  CHECK(scene_node->type() == kObjectSceneNode
        || scene_node->type() == kSceneNode);
  MeshPtr mesh = scene_node->mutable_data()->GetMesh();
  mesh->AddProvider(node_);
  mesh->AddProvider(node_->GetEnvNode());
  mesh_ = mesh;

  bounding_mesh_ = CreateBoundingBoxForSceneNode(scene_node);
  bounding_mesh_->AddProvider(new LoadSceneBVRenderProvider(node_));
  return true;
}

void LordObjectNodeRenderDelegate::Update(const FrameArgs& args) {
  if (mesh_.get()) {
    mesh_->UpdateProviderParams(args);
  }
  bounding_mesh_->UpdateProviderParams(args);
}

void LordObjectNodeRenderDelegate::Render(Renderer* renderer) {
  if (mesh_.get())
    mesh_->Render(renderer);

  SceneNode* scene_node = GetSceneNode();
  if (scene_node->is_draw_bounding_volumn()) {
    bounding_mesh_->Render(renderer);
  }
}

// class LordLampNodeRenderDelegate
LordLampNodeRenderDelegate::LordLampNodeRenderDelegate(SceneRenderNode* node)
    : SceneRenderNodeDelegate(node),
      controller_(NULL) {
  SceneNode* scene_node = GetSceneNode();
  CHECK(scene_node->type() == kLampSceneNode);
  CHECK(scene_node->parent() && scene_node->parent()->type() == kEnvSceneNode);
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


scoped_ptr<SceneRenderNodeDelegate>
LoadSceneRenderNodeDelegateFactory::CreateDelegate(SceneRenderNode* node) {
  switch (node->GetSceneNode()->type()) {
    case kEnvSceneNode:
    case kSceneNode:
      return NULL;
    case kObjectSceneNode:
      return scoped_ptr<SceneRenderNodeDelegate>(
          new LordObjectNodeRenderDelegate(node)).Pass();
    case kLampSceneNode:
      return scoped_ptr<SceneRenderNodeDelegate>(
          new LordLampNodeRenderDelegate(node)).Pass();
    default:
      NOTREACHED() << "no such type supported: " << node->GetSceneNode()->type();
      return scoped_ptr<SceneRenderNodeDelegate>().Pass();
  }
}

// class SimpleRenderTreeRenderer
SimpleRenderTreeRenderer::SimpleRenderTreeRenderer(SceneRenderNode* root)
    : root_(root) {
}

void SimpleRenderTreeRenderer::Update(const FrameArgs& args) {
  UpdateNode(root_, args);
}

void SimpleRenderTreeRenderer::Render(Renderer* renderer) {
  blending_node_.clear();
  RenderNode(root_, renderer);

  {
    ScopedDepthBuffer(false, renderer);
    for (auto iter = blending_node_.begin(); iter != blending_node_.end(); ++iter) {
      (*iter)->Render(renderer);
    }
  }

  for (auto iter = blending_node_.begin(); iter != blending_node_.end(); ++iter) {
    (*iter)->Render(renderer);
  }
}

void SimpleRenderTreeRenderer::UpdateNode(SceneRenderNode* node, 
                                          const FrameArgs& args) {
  node->Update(args);
  for (auto iter = node->children().begin(); 
       iter != node->children().end(); ++iter) {
    UpdateNode(iter->get(), args);
  }
}

void SimpleRenderTreeRenderer::RenderNode(SceneRenderNode* node, 
                                          Renderer* renderer) {
  if (!node->GetSceneNode()->visible()) {
    return;
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
}

MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node) {
  Context* ctx = Context::instance();
  EffectPtr effect = ctx->GetEffect(DiffuseEffect::kEffectName);
  BoxObject* objptr = new BoxObject(effect->GetVertexDesc());
  
  MeshPartPtr objpart = objptr->CreateObject(effect.get());
  BlendingPtr blending = ctx->GetDefaultBlending();
  objpart->SetBlending(blending);
  MeshPartPtr framepart = objptr->CreateFrameObject(effect.get());
  MeshPtr mesh(new Mesh(ctx->GetEffectAdapterContext()));
  mesh->AddMeshPart(framepart);
  mesh->AddMeshPart(objpart);
  return mesh;
}
}  // namespace lord
