
struct VsOutput {
  float4 position : SV_POSITION;
  float4 npos     : NORMAL;
};

struct GsOutput {
  float4 position:SV_POSITION;
};

[maxvertexcount(6)]
void gs_main(triangle VsOutput v[3], inout LineStream<GsOutput> linestream) {
  linestream.RestartStrip();

  GsOutput o;
  [unroll]
  for (int i = 0; i < 3; ++i) {
    o.position = v[i].position;
    linestream.Append(o);
    o.position = v[i].npos;
    linestream.Append(o);
    break;
  }
}