
struct VsOutput {
  float4 position:SV_POSITION;
  float4 normal:NORMAL;
};

struct GsOutput {
  float4 position:SV_POSITION;
};

cbuffer c_buffer {
   float linelength;
};

[maxvertexcount(6)]
void gsmain(triangle VsOutput v[3], inout LineStrip<GsOutput> linestream) {
  linestream.RestartStrip();

  GsOutput o;
  [unroll]
  for (int i = 0; i < 3; ++i) {
    o.position = v[i].position;
    linestream.Append(o);
    o.position = v[i].position + v[i].normal * linelength;
    linestream.Append(o);
  }
}