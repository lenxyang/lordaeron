#include "lordaeron/util/picking.h"

#include "azer/render/render.h"

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
                  azer::Vector3* pt, bool *p) {
  bool parallel = false;
  if (std::abs(ray.directional().dot(plane.normal()) - 1.0f) < 0.001) {
    if (parallel) 
      parallel = true;
  }
    
  *pt = plane.intersect(ray);
  if (p)
    *p = parallel;
}
}  // namespace lord
