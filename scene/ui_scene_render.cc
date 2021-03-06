#include "lordaeron/scene/ui_scene_render.h"

#include "azer/render/render.h"
#include "azer/render/geometry.h"
#include "lordaeron/env.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/effect/normal_line_effect.h"
#include "lordaeron/interactive/light_controller.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/render_env_node.h"
#include "lordaeron/scene/scene_node.h"


namespace lord {
using namespace azer;
const EffectAdapterKey LordSceneBVParamsAdapter::kAdapterKey =
    std::make_pair(typeid(DiffuseEffect).name(),
                   typeid(LordSceneBVRenderProvider).name());

EffectAdapterKey LordSceneBVParamsAdapter::key() const {
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
LordSceneBVRenderProvider::LordSceneBVRenderProvider(RenderNode* node)
    : color_(Vector4(1.0f, 0.0f, 0.0f, 0.3f)), node_(node) {}

void LordSceneBVRenderProvider::Update() {
  SceneNode* snode = node_->GetSceneNode();
  Vector3 vmin, vmax;
  GetSceneNodeBounds(snode, &vmin, &vmax);
  Vector3 center = (vmin + vmax) * 0.5f;
  Vector3 scale(vmax.x - vmin.x, vmax.y - vmin.y, vmax.z - vmin.z);
  scale_ = std::move(Scale(scale));
  world_ = std::move(Translate(center) * scale_);
}

const Matrix4& LordSceneBVRenderProvider::GetPV() const {
  return node_->GetPV();
}

// class LordObjectNodeRenderDelegate
LordObjectNodeRenderDelegate::LordObjectNodeRenderDelegate(
    RenderNode* node, UISceneRender* renderer)
    : RenderNodeDelegate(node),
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
    if (node_->GetEnvNode()) {
      mesh_->AddProvider(node_->GetEnvNode()->delegate());
    }
  }

  bvprovider_ = new LordSceneBVRenderProvider(node_);
  bounding_mesh_ = CreateBoundingBoxForSceneNode(scene_node);
  bounding_mesh_->AddProvider(bvprovider_);
  normal_mesh_ = CreateNormalLineMeshForSceneNode(scene_node);
  if (normal_mesh_.get())
    normal_mesh_->AddProvider(node_);
  return true;
}

void LordObjectNodeRenderDelegate::Update(const FrameArgs& args) {
  bvprovider_->Update();
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
LordLampNodeRenderDelegate::LordLampNodeRenderDelegate(RenderNode* node,
                                                       UISceneRender* renderer)
    : RenderNodeDelegate(node),
      controller_(NULL),
      tree_renderer_(renderer) {
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

  Vector3 vmin = controller_->GetLightMesh()->vmin();
  Vector3 vmax = controller_->GetLightMesh()->vmax();
  scene_node->SetLocalBounds(vmin, vmax);
  bvprovider_ = new LordSceneBVRenderProvider(node_);
  bounding_mesh_ = CreateBoundingBoxForSceneNode(scene_node);
  bounding_mesh_->AddProvider(bvprovider_);
  return true;
}

void LordLampNodeRenderDelegate::Update(const FrameArgs& args) {
  controller_->Update(args);
  bvprovider_->Update();
  SceneNode* scene_node = GetSceneNode();
  if (scene_node->is_draw_bounding_volumn()) {
    tree_renderer_->AddBoundingVolumnMesh(bounding_mesh_);
  }
}

void LordLampNodeRenderDelegate::Render(Renderer* renderer) {
  controller_->Render(renderer);
  
}
  
namespace {
class TreeBuildDelegate : public RenderTreeBuilderDelegate {
 public:
  TreeBuildDelegate(UISceneRender* renderer)
      : tree_renderer_(renderer) {}
  bool NeedRenderNode(SceneNode* node) override { return true;}
  scoped_ptr<RenderNodeDelegate> CreateRenderDelegate(RenderNode* node) override;
  RenderEnvNodeDelegatePtr CreateEnvDelegate(RenderEnvNode* n) override {
    return RenderEnvNodeDelegatePtr(new LordEnvNodeDelegate(n));
  }
 private:
  UISceneRender* tree_renderer_;
};
scoped_ptr<RenderNodeDelegate>
TreeBuildDelegate::CreateRenderDelegate(RenderNode* node) {
  switch (node->GetSceneNode()->type()) {
    case kEnvSceneNode:
      return NULL;
    case kSceneNode:
    case kObjectSceneNode:
      return scoped_ptr<RenderNodeDelegate>(
          new LordObjectNodeRenderDelegate(node, tree_renderer_)).Pass();
    case kLampSceneNode:
      return scoped_ptr<RenderNodeDelegate>(
          new LordLampNodeRenderDelegate(node, tree_renderer_)).Pass();
    default:
      NOTREACHED() << "no such type supported: " << node->GetSceneNode()->type();
      return scoped_ptr<RenderNodeDelegate>().Pass();
  }
}
}

// class UISceneRender
UISceneRender::UISceneRender() {
  scoped_ptr<TreeBuildDelegate> delegate(new TreeBuildDelegate(this));
  SetTreeBuildDelegate(delegate.Pass());
  depth_state_ = RenderSystem::Current()->CreateDepthStencilState();
  depth_state_->EnableDepthTest(false);
}

void UISceneRender::OnFrameUpdateBegin(const FrameArgs& args) {
  blending_node_.clear();
  bvmesh_.clear();
  normal_mesh_.clear();
}

void UISceneRender::OnFrameRenderBegin(Renderer* renderer) {
}

void UISceneRender::OnFrameUpdateEnd(const FrameArgs& args) {
}

void UISceneRender::OnFrameRenderEnd(Renderer* renderer) {
  for (auto iter = blending_node_.begin(); iter != blending_node_.end(); ++iter) {
    (*iter)->Render(renderer);
  }

  for (auto iter = bvmesh_.begin(); iter != bvmesh_.end(); ++iter) {
    (*iter)->Render(renderer);
  }
}

bool UISceneRender::OnUpdateNode(RenderNode* node, const FrameArgs& args) {
  node->Update(args);
  return true;
}

bool UISceneRender::OnRenderNode(RenderNode* node, Renderer* renderer) {
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
    OnRenderNode(iter->get(), renderer);
  }

  return true;
}

MeshPtr CreateBoundingBoxForSceneNode(SceneNode* node) {
  LordEnv* ctx = LordEnv::instance();
  EffectPtr effect = ctx->GetEffect(DiffuseEffect::kEffectName);
  MeshPartPtr objpart = CreateBoxMeshPart(effect, Matrix4::kIdentity);
  objpart->SetEffect(effect);
  BlendingPtr blending = ctx->GetDefaultBlending();
  objpart->SetBlending(blending);
  MeshPartPtr framepart = CreateBoxFrameMeshPart(effect, Matrix4::kIdentity);
  framepart->SetEffect(effect);
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

// class LordEnvNodeDelegate
LordEnvNodeDelegate::LordEnvNodeDelegate(RenderEnvNode* envnode)
    : RenderEnvNodeDelegate(envnode) {
}

void LordEnvNodeDelegate::Init(SceneNode* scene_node, RenderNode* node) {
  int32 child_count = scene_node->child_count();
  for (int32 i = 0; i < child_count; ++i) {
    SceneNode* child = scene_node->child_at(i);
    if (child->type() == kLampSceneNode) {
      light_nodes_.push_back(child);
    }
  }
}

void LordEnvNodeDelegate::OnUpdateNode(const FrameArgs& args) {
  all_lights_.clear();
  RenderEnvNode* parent = node()->parent();
  if (parent) {
    LordEnvNodeDelegate* pdel = static_cast<LordEnvNodeDelegate*>(parent->delegate());
    all_lights_ = pdel->all_lights_;
  }

  for (auto iter = light_nodes_.begin(); iter != light_nodes_.end(); ++iter) {
    SceneNode* node = iter->get();
    DCHECK(node->type() == kLampSceneNode);
    Light* light = node->mutable_data()->light();
    light->set_enable(node->visible());
    all_lights_.push_back(node->mutable_data()->light());
  }
}
}  // namespace lord
