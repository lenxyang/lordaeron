
#pragma pack_matrix(row_major)

float3 NormalSampleToWorldSpace(float3 normal, float3 T, float3 N, float3 B) {
  float3 n = float3(normal * 2.0 - 1.0f);
  float3 r = float3(0, 0, 0);
  r.x = dot(T, n);
  r.y = dot(B, n);
  r.z = dot(N, n);
  return r;
}

Vector3 CalcBinormal(float3 normal, float3 tangent) {
  return cross(tangent, normal);
}
