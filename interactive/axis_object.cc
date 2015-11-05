#include "lordaeron/interactive/axis_object.h"

#include "azer/render/render.h"
#include "lordaeron/context.h"
#include "lordaeron/interactive/controller_object.h"
#include "lordaeron/util/picking.h"

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
  line_ = CreateLineList(pos, (int32)arraysize(pos), desc);
}

// class XYZAxisObject
XYZAxisObject::XYZAxisObject(DiffuseEffect* effect) 
    : effect_(effect) {
  object_.reset(new AxisObject(effect->GetVertexDesc()));
  SetPosition(Vector3(0.0f, 0.0f, 0.0f));
  rotation_[0] = std::move(RotateZ(Degree(90.0f)));
  rotation_[1] = Matrix4::kIdentity;
  rotation_[2] = std::move(RotateX(Degree(-90.0f)));
  reset_color();
}

void XYZAxisObject::set_color(const azer::Vector4& col, int32 index) {
  color_[index] = col;
}

void XYZAxisObject::reset_color() {
  color_[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
  color_[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
  color_[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
}

void XYZAxisObject::set_length(float length) {
  object_->set_length(length);
}

void XYZAxisObject::SetPosition(const azer::Vector3& pos) {
  position_ = pos;
}

void XYZAxisObject::SetPV(const azer::Matrix4& pv) {
  pv_ = pv;
}

void XYZAxisObject::Render(azer::Renderer* renderer) {
  Context* context = Context::instance();
  Matrix4 world = std::move(Translate(position_));
  for (int32 i = 0; i < 3; ++i) {
    Matrix4 lworld = std::move(world * rotation_[i]);
    effect_->SetDirLight(context->GetInternalLight());
    effect_->SetColor(color_[i]);
    effect_->SetPV(pv_);
    effect_->SetWorld(lworld);
    renderer->UseEffect(effect_.get());
    object_->Render(renderer);
  }
}

int32 XYZAxisObject::Picking(const azer::Ray& ray) {
  const Vector3& pos = position_;
  Plane pxy(Vector3(0.0f, 0.0f, 1.0f), -pos.z);
  Plane pyz(Vector3(1.0f, 0.0f, 0.0f), -pos.x);
  Plane pzx(Vector3(0.0f, 1.0f, 0.0f), -pos.y);

  const float kMargin = 0.05f;
  bool parallel;
  float length = object_->length();
  Vector3 pt;
  {
    PickingPlane(ray, pxy, &pt, &parallel);
    if (!parallel) {
      if (pt.x - pos.x <= length && pt.x - pos.x > 0.0 &&
          std::abs(pt.y - pos.y) < 0.01 && std::abs(pt.y - pos.y) < kMargin) {
        return kPickXAxis;
      } else if (pt.y - pos.y <= length && pt.y - pos.y > 0.0f &&
                 std::abs(pt.x - pos.x) < 0.01 && std::abs(pt.x - pos.x) < kMargin) {
        return kPickYAxis;
      }
    }
  }

  {
    // yzplane
    PickingPlane(ray, pyz, &pt, &parallel);
    if (!parallel) {
      if (pt.y - pos.y <= length && pt.y - pos.y > 0.0 &&
                 std::abs(pt.z - pos.z) < 0.01 && std::abs(pt.z - pos.z) < kMargin) {
        return kPickYAxis;
      } else if (pt.z - pos.z <= length && pt.z - pos.z > 0.0f &&
                 std::abs(pt.y - pos.y) < 0.01 && std::abs(pt.y - pos.y) < kMargin) {
        return kPickZAxis;
      }
    }
  }

  {
    // zxplane
    PickingPlane(ray, pzx, &pt, &parallel);
    if (!parallel) {
      if (pt.z - pos.z <= length && pt.z - pos.z > 0.0 &&
                 std::abs(pt.x - pos.x) < 0.01 && std::abs(pt.x - pos.x) < kMargin) {
        return kPickZAxis;
      } else if (pt.x - pos.x <= length && pt.x - pos.x > 0.0f &&
                 std::abs(pt.z - pos.z) < 0.01 && std::abs(pt.z - pos.z) < kMargin) {
        return kPickXAxis;
      }
    }
  }
  return kPickNone;
}
}  // namespace lord
