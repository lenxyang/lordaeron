#include "lordaeron/interactive/controller_object.h"

#include "azer/render/render.h"
#include "lordaeron/context.h"

namespace lord {
using namespace azer;
EntityPtr CreatePlane(const Vector4 pos[4], VertexDesc* desc) {
  VertexPos normal_pos;
  bool kHasNormal0Idx = GetSemanticIndex("normal", 0, desc, &normal_pos);
  SlotVertexDataPtr vdata = new SlotVertexData(desc, 6);
  VertexPack vpack(vdata.get());
  
  vpack.first();
  int32 indices[6] = {0, 2, 3, 0, 2, 1};
  for (uint32 i = 0; i < arraysize(indices); ++i) {
    int32 idx = indices[i];
    vpack.WriteVector4(pos[idx], VertexPos(0, 0));
    if (kHasNormal0Idx)
      vpack.WriteVector4(Vector4(1.0f, 1.0f, 1.0f, 0.0f), normal_pos);
    vpack.next(1);
  }

  RenderSystem* rs = RenderSystem::Current();
  VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
  EntityPtr entity = new Entity;
  entity->SetVertexBuffer(vb);
  return entity;
}

EntityPtr CreateLineList(const Vector4* pos, int32 count, VertexDesc* desc) {
  Vector4 normal(1.0f, 1.0f, 1.0f, 0.0f);
  VertexPos normal_pos;
  bool kHasNormal0Idx = GetSemanticIndex("normal", 0, desc, &normal_pos);
  SlotVertexDataPtr vdata = new SlotVertexData(desc, count);
  VertexPack vpack(vdata.get());
  vpack.first();
  for (int32 i = 0; i < count; ++i) {
    vpack.WriteVector4(pos[i], VertexPos(0, 0));
    if (kHasNormal0Idx)
      vpack.WriteVector4(normal, normal_pos);
    vpack.next(1);
  }

  RenderSystem* rs = RenderSystem::Current();
  VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
  EntityPtr entity = new Entity;
  entity->set_topology(kLineList);
  entity->SetVertexBuffer(vb);
  return entity;
}

// class ControllerObject
ControllerObject::ControllerObject(DiffuseEffect* effect)
    : effect_(effect) {
}

ControllerObject::~ControllerObject() {
}

void ControllerObject::SetPosition(const azer::Vector3& pos) {
  world_ = Translate(pos);
}
}  // namespace lord
