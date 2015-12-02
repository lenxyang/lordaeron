#pragma once

#include "base/memory/ref_counted.h"
#include "azer/math/math.h"
#include "azer/render/render.h"
#include "lordaeron/scene/scene_node_observer.h"

namespace lord {
struct Attenuation {
  azer::Vector3 coefficient; 
  float range;
};

struct DirLight {
  azer::Vector4 diffuse;
  azer::Vector4 ambient;
  azer::Vector4 specular;
  azer::Vector3 direction;
  float pad;
};

struct PointLight {
  azer::Vector4 diffuse;
  azer::Vector4 ambient;
  azer::Vector4 specular;
  azer::Vector3 position;
  float pad;
  Attenuation atten;
};

struct SpotLight {
  azer::Vector4 diffuse;
  azer::Vector4 ambient;
  azer::Vector4 specular;
  azer::Vector3 position;
  float phi;
  azer::Vector3 direction;
  float theta;  // inner corner
  Attenuation atten;
};

enum LightType {
  kDirectionalLight,
  kPointLight,
  kSpotLight,
};

class Light : public ::base::RefCounted<Light>,
              public SceneNodeObserver {
 public:
  explicit Light(const DirLight& light);
  explicit Light(const PointLight& light);
  explicit Light(const SpotLight& light);
  LightType type() const { return type_;}

  const azer::Vector4& diffuse() const;
  const azer::Vector4& ambient() const;
  const azer::Vector4& specular() const;

  const DirLight& dir_light() const;
  DirLight* mutable_dir_light();

  const PointLight& point_light() const;
  PointLight* mutable_point_light();

  const SpotLight& spot_light() const;
  SpotLight* mutable_spot_light();

  azer::Mesh* GetLightMesh() { return mesh_.get();}

  // override form SceneNodeObserver
  void OnSceneNodeOrientationChanged(
      SceneNode* node, const azer::Quaternion& prev_orient) override;
  void OnSceneNodeLocationChanged(
      SceneNode* node, const azer::Vector3& prevpos) override;
 private:
  LightType type_;
  DirLight dir_light_;
  PointLight point_light_;
  SpotLight spot_light_;
  azer::MeshPtr mesh_;
  DISALLOW_COPY_AND_ASSIGN(Light);
};

typedef scoped_refptr<Light> LightPtr;
}  // namespace lord
