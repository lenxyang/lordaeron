#pragma pack_matrix(row_major)

struct DirLight {
  float4 diffuse;
  float4 ambient;
  float4 specular;
  float3 dir;
  float  pad;
};

struct Attenuation {
  float a;
  float b;
  float c;
  float range;
};

struct PointLight {
  float4      diffuse;
  float4      ambient;
  float4      specular;
  float3      position;
  float       pad;
  Attenuation atten;
};

struct SpotLight {
  float4      diffuse;
  float4      ambient;
  float4      specular;
  float3      position;
  float       phi;
  float3      direction;
  float       theta;
  Attenuation atten;
};

float3 CalcSpecularIntensity(float3 ldir, float3 normal, float3 viewin, int p) {
  float3 reflect = normalize(2.0f * dot(normal, ldir) * normal - ldir);
  float3 specular_intensity = pow(p, max(0, dot(reflect, viewin)));
  return specular_intensity;
}

float3 CalcDirLightIntensity(DirLight light, float3 normal, float3 viewin, int p) {
  float3 ldir = normalize(light.dir.xyz);
  float diffuse_intensity = max(0.0, dot(normal, -ldir));
  float3 light_intensity = light.ambient.xyz
      + light.diffuse.xyz * diffuse_intensity
      + CalcSpecularIntensity(ldir, normal, viewin, p) * light.specular;
  return light_intensity;
}
