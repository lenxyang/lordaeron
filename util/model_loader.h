#pragma once

#include "azer/render/render.h"
#include "azer/files/file_system.h"

namespace lord {
class ModelLoader {
 public:
  ModelLoader(azer::FileSystem* fs);

  azer::MeshPtr Load(const azer::ResPath& path, azer::VertexDescPtr desc);
 private:
  azer::FileSystem* fsystem_;
  DISALLOW_COPY_AND_ASSIGN(ModelLoader);
};
}  // namespace lord
