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

float phong(float3 ldir, float3 normal, float3 viewin) {
  float3 r = normalize(2.0f * dot(normal, ldir) * normal - ldir);
  return max(0, dot(r, viewin));
}

float blinn_phong(float3 ldir, float3 normal, float3 viewin) {
  float half_vec = normalize(ldir + viewin);
  return max(0, dot(half_vec, normal));
}

float3 CalcDirLightColor(DirLight light, float3 normal, float3 viewin, Matrial m) {
  float3 ldir = normalize(light.dir.xyz);
  float3 diffuse = max(0.0, dot(normal, -ldir)) * 
                  light.diffuse.xyz * m.diffuse.xyz;
  float3 ambient = m.ambient.xyz * light.ambient.xyz;
  float3 specular = pow(m.power, blinn_phong(ldir, normal, viewin))
                 * light.specular.xyz * m.specular.xyz;

  return float3(ambient.xyz + diffuse.xyz + specular.xyz + m.emission.xyz);
}

float CalcAttenuation(float dist, Attenuation atten) {
  float att = 1.0f / (atten.a * dist * dist + atten.b * dist + atten.c);
  return step(dist, atten.range) * saturate(att);
}

float3 CalcPointLightColor(PointLight light, float3 pos, float3 normal, 
                           float3 viewin, Matrial mtrl) {
  float3 ldir = normalize(pos - light.position);
  float dist = distance(pos, light.position);
  float3 diffuse = max(0.0, dot(normal, -ldir)) * 
                  light.diffuse.xyz * mtrl.diffuse.xyz;
  float atten = CalcAttenuation(dist, light.atten);
  float3 ambient = mtrl.ambient.xyz * light.ambient.xyz;
  float3 specular = pow(mtrl.power, blinn_phong(ldir, normal, viewin))
                 * light.specular.xyz * mtrl.specular.xyz;

  return float3(ambient.xyz + (diffuse.xyz + specular.xyz) * atten
                + mtrl.emission.xyz);
}

