#include "lordaeron/scene/lord_scene_render.h"

#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/light_mesh.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_render_tree.h"

namespace lord {
using namespace azer;
LordObjectNodeRenderDelegate::LordObjectNodeRenderDelegate(SceneRenderNode* node)
    : SceneRenderNodeDelegate(node) {
}

bool LordObjectNodeRenderDelegate::Init() {
  CHECK(scene_node->type() == kObjectSceneNode);
  SceneNode* scene_node = GetSceneNode();
  MeshPtr mesh = scene_node->mutable_data()->GetMesh();
  mesh->AddProvider(node_);
  mesh->AddProvider(node_->GetEnvNode());
  mesh_ = mesh;
  return true;
}

void LordObjectNodeRenderDelegate::Update(const FrameArgs& args) {
  if (mesh_.get()) {
    mesh_->UpdateProviderParams(args);
  }
}

void LordObjectNodeRenderDelegate::Render(Renderer* renderer) {
  if (mesh_.get())
    mesh_->Render(renderer);
}

// class LordLampNodeRenderDelegate
LordLampNodeRenderDelegate::LordLampNodeRenderDelegate(SceneRenderNode* node)
    : SceneRenderNodeDelegate(node) {
  SceneNode* scene_node = GetSceneNode();
  CHECK(GetSceneNode->type() == kLampSceneNode);
  CHECK(scene_node->parent() && scene_node->parent()->type() == kEnvSceneNode);
}

bool LordLampNodeRenderDelegate::Init() {
  Light* light = scene_node->mutable_data()->light();
  LightMeshPtr mesh = CreateLightMesh(scene_node);
  mesh->AddProvider(node_);
  mesh->AddProvider(node_->GetEnvNode());
  // LightMeshProvider must be put last, because it will override
  // the world matrix
  mesh->AddProvider(new LightMeshProvider(scene_node, mesh->local_transform()));
  mesh_ = mesh;
  return true;
}

void LordLampNodeRenderDelegate::Update(const FrameArgs& args) {
  if (mesh_.get()) {
    mesh_->UpdateProviderParams(args);
  }
}

void LordLampNodeRenderDelegate::Render(Renderer* renderer) {
  if (mesh_.get())
    mesh_->Render(renderer);
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
