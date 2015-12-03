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

struct Matrial {
  float4 diffuse;
  float4 ambient;
  float4 specular;
  float4 emission;
  float  power;
  float  alpha;
  float  pad1;
  float  pad2;
};

float3 CalcSpecularIntensity(float3 ldir, float3 normal, float3 viewin, int p) {
  float3 reflect = normalize(2.0f * dot(normal, ldir) * normal - ldir);
  float3 specular_intensity = pow(p, max(0, dot(reflect, viewin)));
  return specular_intensity;
}

float3 CalcDirLightColor(DirLight light, float3 normal, float3 viewin, 
                         Matrial mtrl) {
  float3 ldir = normalize(light.dir.xyz);
  float3 diffuse = max(0.0, dot(normal, -ldir)) * 
                  light.diffuse.xyz * mtrl.diffuse.xyz;
  float3 ambient = mtrl.ambient.xyz * light.ambient.xyz;
  float3 specular = CalcSpecularIntensity(ldir, normal, viewin, mtrl.power) 
                 * light.specular.xyz * mtrl.specular.xyz;
  
  return float3(diffuse.xyz + ambient.xyz + specular.xyz + mtrl.emission.xyz);
}
