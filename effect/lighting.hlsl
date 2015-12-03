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
