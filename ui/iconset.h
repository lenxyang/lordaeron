#pragma once

#include "base/basictypes.h"
#include "ui/gfx/image/image_skia.h"
#include "nelf/nelf.h"

namespace lord {
class Context;

class Iconset {
 public:
  enum {
    kIconSceneNode        = 0x00000017,
    kIconSceneCamera      = 0x00060014,
    kIconSceneLamp        = 0x00050014,
    kIconSceneObject      = 0x00030017,
    kIconSceneWorld       = 0x00020017,
    kIconSceneTemplate    = 0x00010017,
    kIconSceneMtrl        = 0x00160016,

    kIconTriangleLeft     = 0x0006001D,
    kIconTriangleRight    = 0x0004001D,
    kIconTriangleUp       = 0x0007001D,
    kIconTriangleDown     = 0x0005001D,
    kIconTriangleLR       = 0x0008001D,
    kIconExpand           = 0x000A001D,
    kIconCollapse         = 0x000B001D,
  };
  Iconset(Context* context);

  gfx::ImageSkia GetIcon(int32 id);
 private:
  scoped_ptr<nelf::IconsetPackage> package_;
  DISALLOW_COPY_AND_ASSIGN(Iconset);
};
}  // namespace lord
