#pragma pack_matrix(row_major)

struct VsOutput {
  float4 position : SV_POSITION;
  float4 npos     : NORMAL;
};

struct VSInput {
  float4 position : POSITION;
  float4 normal   : NORMAL;
};

cbuffer c_buffer {
   float4x4 pvw;
   float4x4 world;
   float linelength;
};

VsOutput vs_main(VSInput input) {
  VsOutput o;
  o.position = mul(pvw, input.position);
  o.npos = mul(pvw, input.position + input.normal * 0.1);
  return o;
}
