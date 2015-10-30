#pragma once

#include "base/basictypes.h"
#include "azer/render/render.h"

namespace lord {

class BoundingVolumn {
 public:
  BoundingVolumn();
  ~BoundingVolumn();

  virtual bool IsIntersect(const azer::Ray& ray) const = 0;
 private:
  DISALLOW_COPY_AND_ASSIGN(BoundingVolumn);
};
}  // namespace lord
