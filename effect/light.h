#pragma once

#include <vector>

#include "base/memory/ref_counted.h"
#include "base/observer_list.h"
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
  float enable;
};

struct PointLight {
  azer::Vector4 diffuse;
  azer::Vector4 ambient;
  azer::Vector4 specular;
  azer::Vector3 position;
  float enable;
  Attenuation atten;
};

struct SpotLight {
  azer::Vector4 diffuse;
  azer::Vector4 ambient;
  azer::Vector4 specular;
  azer::Vector3 position;
  float pad1;
  azer::Vector3 direction;
  float enable;
  float phi;
  float theta;  // inner corner
  float range;
  float falloff;
};

enum LightType {
  kDirectionalLight,
  kPointLight,
  kSpotLight,
};

class Light;

class LightObserver {
 public:
  virtual void OnLightPositionChanged(Light* light) {}
  virtual void OnLightDirectionChanged(Light* light) {}
};

class Light : public ::base::RefCounted<Light> {
 public:
  explicit Light(const DirLight& light);
  explicit Light(const PointLight& light);
  explicit Light(const SpotLight& light);
  LightType type() const { return type_;}

  const azer::Vector4& diffuse() const;
  const azer::Vector4& ambient() const;
  const azer::Vector4& specular() const;
  const azer::Vector3& directional() const;

  bool enable() const;
  void set_enable(bool v);
  void set_diffuse(const azer::Vector4& color);
  void set_ambient(const azer::Vector4& color);
  void set_specular(const azer::Vector4& color);
  void set_position(const azer::Vector3& pos);
  void set_directional(const azer::Vector3& dir);

  const DirLight& dir_light() const;
  const PointLight& point_light() const;
  const SpotLight& spot_light() const;
  void SetDirLight(const DirLight& l);
  void SetPointLight(const PointLight& l);
  void SetSpotLight(const SpotLight& l);

  void AddObserver(LightObserver* observer);
  void RemoveObserver(LightObserver* observer);
  bool HasObserver(LightObserver* observer) const;

  azer::Texture* shadowmap() { return shadowmap_;}
  void SetShadowMap(azer::Texture* tex) { shadowmap_ = tex;}
 private:
  LightType type_;
  DirLight dir_light_;
  PointLight point_light_;
  SpotLight spot_light_;
  azer::TexturePtr shadowmap_;
  ObserverList<LightObserver> observers_;
  DISALLOW_COPY_AND_ASSIGN(Light);
};

typedef scoped_refptr<Light> LightPtr;
typedef std::vector<LightPtr> Lights;
}  // namespace lord
