#pragma once

#include "base/memory/ref_counted.h"
#include "azer/math/math.h"
#include "azer/render/render.h"
#include "azer/render/effect_creator.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/effect/material.h"

namespace lord {
namespace sandbox {
class ColorMaterial : public Material {
 public:
  ColorMaterial() {}
  static const char kEffectProviderName[];
  const char* GetProviderName() const override { return kEffectProviderName;}
  bool Init(const azer::ConfigNode* node, ResourceLoadContext* ctx) override {
    CHECK(node->GetChildTextAsVec4("color", &color_));
    return true;
  }

  const azer::Vector4& color() const { return color_;}
  static azer::EffectParamsProvider* CreateObject() { return new ColorMaterial;}
 private:
  azer::Vector4 color_;
  DECLARE_EFFECT_PROVIDER_DYNCREATE(ColorMaterial);
  DISALLOW_COPY_AND_ASSIGN(ColorMaterial);
};

class MyEffect : public azer::Effect {
 public:
  static const char kEffectName[];
  MyEffect();
  ~MyEffect();

  const char* GetEffectName() const override;
  bool Init(const ShaderPrograms& source) override;

#pragma pack(push, 4)
  struct vs_cbuffer {
    azer::Matrix4 pvw;
    azer::Matrix4 world;
    azer::Vector4 camerapos;
  };

  struct ps_cbuffer {
    DirLight light;
    PointLight pointlight;
    SpotLight  spotlight;
    azer::Vector4 color;
  };
#pragma pack(pop)

  void SetPV(const azer::Matrix4& value);
  void SetWorld(const azer::Matrix4& value);
  void SetCameraPos(const azer::Vector4& CameraPos);
  void SetColor(const azer::Vector4& value);
  void SetDirLight(const DirLight& value);
  void SetPointLight(const PointLight& value);
  void SetSpotLight(const SpotLight& value);
  static Effect* CreateObject() { return new MyEffect;}
 protected:
  void ApplyGpuConstantTable(azer::Renderer* renderer) override;
  void InitTechnique(const ShaderPrograms& source);
  void InitGpuConstantTable();

  azer::Matrix4 pv_;
  azer::Matrix4 world_;
  azer::Vector4 color_;
  azer::Vector4 camerapos_;
  DirLight dir_light_;
  PointLight point_light_;
  SpotLight spot_light_;

  DECLARE_EFFECT_DYNCREATE(MyEffect);
  DISALLOW_COPY_AND_ASSIGN(MyEffect);
};

typedef scoped_refptr<MyEffect> MyEffectPtr;
MyEffectPtr CreateMyEffect();
}  // namespace sandbox
}  // namespace lord
