#pragma once

#include "base/memory/ref_counted.h"
#include "azer/render/render.h"

namespace lord {

class SceneNode;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class SceneEnvironment;
class GlobalEnvironmentParams;
typedef scoped_refptr<GlobalEnvironmentParams>  GlobalEnvironmentParamsPtr;

class SceneContext : public ::base::RefCounted<SceneContext> {
 public:
  SceneContext();
  ~SceneContext();

  SceneEnvironment* GetNodeEnvironment();
  GlobalEnvironmentParams* GetGlobalEnvironment();
 private:
  scoped_ptr<SceneEnvironment> node_environment_;
  GlobalEnvironmentParamsPtr global_environment_;
  DISALLOW_COPY_AND_ASSIGN(SceneContext);
};

typedef scoped_refptr<SceneContext> SceneContextPtr;
}  // namespace lord
