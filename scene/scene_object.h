#pragma once

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"

namespace lord {
class SceneObject : public ::base::RefCounted<SceneObject> {
 public:
  enum Type {
    kObjectNode,
    kLampNode,
    kCameraNode,
    kTerrainTileNode,
  };

  SceneObject(Type type);
  ~SceneObject();

  Type type() const { return type_;}
 private:
  Type type_;
  DISALLOW_COPY_AND_ASSIGN(SceneObject);
};

typedef scoped_refptr<SceneObject> SceneObjectPtr;
  
}  // namespace lord
