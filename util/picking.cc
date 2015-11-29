#include "lordaeron/util/picking.h"

#include "azer/render/render.h"
#include "azer/render/util.h"
#include "lordaeron/context.h"

namespace lord {
using namespace azer;
Vector4 CalcWorldPosFromScreen(const gfx::Point& pt, const gfx::Size& size, 
                               const Camera* camera) {
  float x = 2 * (float)pt.x() / (float)size.width() - 1.0f;
  float y = -2 * (float)pt.y() / (float)size.height() + 1.0f;
  Vector4 pos(x, y, 0.0f, 1.0f);

  const Matrix4& proj = camera->frustrum().projection();
  const Matrix4& view = camera->GetViewMatrix();
  Matrix4 mat = std::move(view.InverseCopy() * proj.InverseCopy());
  
  pos = mat * pos;
  pos /= pos.w;
  return pos;
}

Ray GetPickingRay(const gfx::Point& pt, const gfx::Size& size,
                  const Camera* camera) {
  // another method
  // reference 3D Game engine design(2nd edition) 8.4, Object Picking
  Vector4 pos = Vector4(camera->holder().position(), 1.0f);
  Vector4 pos2 = CalcWorldPosFromScreen(pt, size, camera);
  Vector4 dir = pos - pos2;
  Vector3 nd(dir.x, dir.y, dir.z);
  nd.Normalize(); 
  return Ray(Vector3(pos.x, pos.y, pos.z), nd);
}

void PickingPlane(const azer::Ray& ray, const azer::Plane& plane, 
                  azer::Vector3* pt, const azer::Camera& camera, float* depth, 
                  bool *p) {
  bool parallel = false;
  float dot = ray.directional().dot(plane.normal());
  if (std::abs(dot) < 0.001) {
    parallel = true;
  }
    
  *pt = plane.intersect(ray);
  Vector4 v = camera.GetProjViewMatrix() * Vector4(*pt, 1.0f);
  *depth = v.z / v.w;
  if (p) {
    *p = parallel;
  }
}

// class PlanePickingHelper
PlanePickingHelper::PlanePickingHelper(const azer::Plane& plane)
    : show_picking_(false),
      plane_(plane) {
  effect_ = CreateDiffuseEffect();
  GeometryObjectPtr ptr = new SphereObject(effect_->GetVertexDesc(), 32, 32);
  sphere_ = new Entity(ptr->GetVertexBuffer(), ptr->GetIndicesBuffer());
  CreatePlane();
}

void PlanePickingHelper::CreatePlane() {
  float d = 100.0f;
  Vector4 pos[4] = {
    Vector4(-d,  d,  d, 1.0f),
    Vector4( d,  d, -d, 1.0f),
    Vector4( d, -d, -d, 1.0f),
    Vector4(-d, -d,  d, 1.0f),
  };
  if (std::abs(plane_.normal().x - 1.0f) < 0.001) {
    for (int i = 0; i < 4; ++i) {
      pos[i].x = -plane_.d();
    }
  } else if (std::abs(plane_.normal().y - 1.0f) < 0.001) {
    for (int i = 0; i < 4; ++i) {
      pos[i].y = -plane_.d();
    }
  } else if (std::abs(plane_.normal().z - 1.0f) < 0.001) {
    for (int i = 0; i < 4; ++i) {
      pos[i].z = -plane_.d();
    }
  } else {
  }
}

void PlanePickingHelper::SetPickingPos(const azer::Vector3& pos) {
  position_ = pos;
}

void PlanePickingHelper::SetShowPicking(bool show_picking) {
  show_picking_ = show_picking;
}

void PlanePickingHelper::Render(const azer::Matrix4& pv, azer::Renderer* renderer) {
}
}  // namespace lord
