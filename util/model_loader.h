#pragma once

#include "azer/render/render.h"
#include "azer/files/file_system.h"

struct aiScene;
struct aiMesh;
namespace Assimp {
class Importer;
}

namespace lord {
class ModelLoader {
 public:
  ModelLoader(azer::FileSystem* fs);

  azer::MeshPtr Load(const azer::ResPath& path, azer::VertexDesc* desc);
  azer::EntityVecPtr LoadVertexData(const azer::ResPath& path, 
                                    azer::VertexDesc* desc);
 private:
  const aiScene* LoadScene(const azer::ResPath& path, Assimp::Importer* importer, 
                           uint32 flags);
  azer::FileSystem* fsystem_;
  DISALLOW_COPY_AND_ASSIGN(ModelLoader);
};
}  // namespace lord
