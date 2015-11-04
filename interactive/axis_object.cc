#include "lordaeron/interactive/axis_object.h"

#include "azer/render/render.h"
#include "lordaeron/context.h"

namespace lord {
using namespace azer;
AxisObject::AxisObject(VertexDesc* desc) 
    : length_(1.0),
      cone_radius_(0.08f),
      cone_height_(0.18f),
      desc_(desc) {
  CreateCone(desc);
  CreateLine(desc);
}

AxisObject::~AxisObject() {
}

void AxisObject::set_length(float length) {
  length_ = length;
  CreateCone(desc_.get());
  CreateLine(desc_.get());
}

void AxisObject::Render(Renderer* renderer) {
  cone_->DrawIndex(renderer);

  bool depth_enable = renderer->IsDepthTestEnable();
  renderer->EnableDepthTest(false);
  line_->Draw(renderer);
  renderer->EnableDepthTest(depth_enable);
}

void AxisObject::CreateCone(VertexDesc* desc) {
  SlotVertexDataPtr vdata = InitConeVertexData(32, VertexDescPtr(desc));
  VertexPack vpack(vdata.get());
  vpack.first();
  Vector4 pos;
  VertexPos posidx(0, 0);
  while (!vpack.end()) {
    vpack.ReadVector4(&pos, posidx);
    pos = Vector4(pos.x * cone_radius_,
                  pos.y * cone_height_,
                  pos.z * cone_radius_, 1.0f);
    pos.y = pos.y + length() - cone_height_;
    vpack.WriteVector4(pos, posidx);
    vpack.next(1);
  }

  IndicesDataPtr idata = InitConeIndicesData(32);
  RenderSystem* rs = RenderSystem::Current();
  VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
  IndicesBufferPtr ib = rs->CreateIndicesBuffer(IndicesBuffer::Options(), idata);
  cone_ = new Entity(vb, ib);
}

void AxisObject::CreateLine(VertexDesc* desc) {
  Vector4 pos[] = {
    Vector4(0.0f, 0.0f, 0.0f, 1.0f),
    Vector4(0.0f, length() - cone_height_, 0.0f, 1.0f),
  };
  Vector4 normal(1.0f, 1.0f, 1.0f, 0.0f);

  VertexPos normal_pos;
  bool kHasNormal0Idx = GetSemanticIndex("normal", 0, desc, &normal_pos);
  SlotVertexDataPtr vdata = new SlotVertexData(desc, arraysize(pos));
  VertexPack vpack(vdata.get());
  vpack.first();
  for (int32 i = 0; i < arraysize(pos); ++i) {
    vpack.WriteVector4(pos[i], VertexPos(0, 0));
    if (kHasNormal0Idx)
      vpack.WriteVector4(normal, normal_pos);
    vpack.next(1);
  }

  RenderSystem* rs = RenderSystem::Current();
  VertexBufferPtr vb = rs->CreateVertexBuffer(VertexBuffer::Options(), vdata);
  line_ = new Entity;
  line_->set_topology(kLineList);
  line_->SetVertexBuffer(vb);
}

// class XYZAxisObject
XYZAxisObject::XYZAxisObject(DiffuseEffect* effect) 
    : effect_(effect) {
  object_.reset(new AxisObject(effect->GetVertexDesc()));
  SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  rotation_[0] = std::move(RotateZ(Degree(90.0f)));
  rotation_[1] = Matrix4::kIdentity;
  rotation_[2] = std::move(RotateX(Degree(-90.0f)));
  ResetAxisColor();
}

void XYZAxisObject::ResetAxisColor() {
  color_[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
  color_[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
  color_[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
}

void XYZAxisObject::set_length(float length) {
  object_->set_length(length);
}

void XYZAxisObject::SetPosition(const azer::Vector3& pos) {
  world_ = Translate(pos);
}

void XYZAxisObject::SetPV(const azer::Matrix4& pv) {
  pv_ = pv;
}

void XYZAxisObject::Render(azer::Renderer* renderer) {
  Context* context = Context::instance();
  for (int32 i = 0; i < 3; ++i) {
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetColor(color_[i]);
    effect_->SetPV(pv_);
    effect_->SetWorld(world_);
    renderer->UseEffect(effect_.get());
    object_->Render(renderer);
  }
}
}  // namespace lord
