#include "lordaeron/util/model_loader.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/anim.h"
#include "azer/render/util.h"
#include "lordaeron/util/bounding_volumn.h"

namespace lord {

namespace {
using namespace azer;
struct MeshData {
  SlotVertexDataPtr vdata;
  IndicesDataPtr idata;
  Vector3 vmin;
  Vector3 vmax;
};

MeshData LoadMeshData(const aiMesh* paiMesh, azer::VertexDescPtr desc) {
  SlotVertexDataPtr vdata(new SlotVertexData(desc, paiMesh->mNumVertices));
  IndicesDataPtr idata(new IndicesData(paiMesh->mNumFaces * 3));
  VertexPack vpack(vdata.get());
  Vector3 vmin  = Vector3(999999.9f, 999999.9f, 999999.9f);
  Vector3 vmax = Vector3(-999999.9f, -999999.9f, -999999.9f);
  VertexPos npos, tpos;
  azer::GetSemanticIndex("normal", 0, desc.get(), &npos);
  azer::GetSemanticIndex("texcoord", 0, desc.get(), &tpos);

  const aiVector3D zero3d(0.0f, 0.0f, 0.0f);
  CHECK(vpack.first());
  for (uint32 i = 0; i < paiMesh->mNumVertices; ++i) {
    const aiVector3D& aipos = paiMesh->mVertices[i];
    const aiVector3D& normal = paiMesh->mNormals[i];
    const aiVector3D& texcoord =
        paiMesh->HasTextureCoords(0) ? (paiMesh->mTextureCoords[0][i]) : zero3d;

    Vector3 pos(aipos.x, aipos.y, aipos.z);
    UpdateVMinAndVMax(pos, &vmin, &vmax);
    vpack.WriteVector4(Vector4(pos, 1.0f), VertexPos(0, 0));
    vpack.WriteVector2(Vector2(texcoord.x, texcoord.y), tpos);
    vpack.WriteVector4(Vector4(normal.x, normal.y, normal.z, 0.0), npos);
    vpack.next(1);
  }

  IndexPack ipack(idata.get());
  for (uint32 i = 0; i < paiMesh->mNumFaces; ++i) {
    const aiFace& face = paiMesh->mFaces[i];
    CHECK(ipack.WriteAndAdvance(face.mIndices[0]));
    CHECK(ipack.WriteAndAdvance(face.mIndices[1]));
    CHECK(ipack.WriteAndAdvance(face.mIndices[2]));
  }

  MeshData data;
  data.vdata = vdata;
  data.idata = idata;
  data.vmax = vmax;
  data.vmin = vmin;
  return data;
}
}  // namespace
  
ModelLoader::ModelLoader(azer::FileSystem* fs)
    : fsystem_(fs) {
}

azer::MeshPtr ModelLoader::Load(const azer::ResPath& path,
                                azer::VertexDescPtr desc) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  Assimp::Importer importer;
  uint32 flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals 
      | aiProcess_FlipUVs;
  std::vector<uint8> contents;
  if (!fsystem_->ReadFile(path, &contents)) {
    return azer::MeshPtr();
  }

  const aiScene* scene = importer.ReadFileFromMemory(
      (const void*)&contents[0], contents.size(), flags, 
      ::base::UTF16ToUTF8(path.filename().as_string()).c_str());
  if (scene == NULL) {
    LOG(ERROR) << "Failed to load file: " << path.fullpath();
    return azer::MeshPtr();
  } 

  azer::MeshPtr mesh(new Mesh);
  std::vector<int32> mtrl_index;
  Vector3 vmin  = Vector3(999999.9f, 999999.9f, 999999.9f);
  Vector3 vmax = Vector3(-999999.9f, -999999.9f, -999999.9f);
  for (uint32 i = 0; i < scene->mNumMeshes; ++i) {
    MeshPartPtr part = new MeshPart(NULL);
    const aiMesh* paiMesh = scene->mMeshes[i];
    MeshData data = LoadMeshData(scene->mMeshes[i], desc);
    azer::EntityPtr entity(new azer::Entity());
    entity->SetVertexBuffer(rs->CreateVertexBuffer(
        VertexBuffer::Options(), data.vdata));
    entity->SetIndicesBuffer(rs->CreateIndicesBuffer(
        IndicesBuffer::Options(), data.idata));
    *entity->mutable_vmin() = data.vmin;
    *entity->mutable_vmax() = data.vmax;
    mesh->AddMeshPart(part);
    mtrl_index.push_back(paiMesh->mMaterialIndex);
    part->AddEntity(entity);
    mesh->AddMeshPart(part);
  }

  return mesh;
}
}  // namespace lord
