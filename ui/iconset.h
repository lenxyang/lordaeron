#pragma once

#include "base/basictypes.h"
#include "ui/gfx/image/image_skia.h"
#include "nelf/nelf.h"

namespace lord {
class Context;

class Iconset {
 public:
  enum {
  };
  Iconset(Context* context);

  gfx::ImageSkia GetIcon(int32 id);
 private:
  scoped_ptr<nelf::IconsetPackage> package_;
  DISALLOW_COPY_AND_ASSIGN(Iconset);
};
}  // namespace lord
