cbuffer UICB : register(b0)
{
    float4 gTint; // rgba, optionnel (1,1,1,1)
};

Texture2D gTex0 : register(t0);

SamplerState gSamp0 : register(s0);

struct VertexIn
{
    float2 pos : POSITION; // NDC [-1..1]
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
};

struct VertexOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
};

VertexOut VSMain(VertexIn v)
{
    VertexOut o;
    o.pos = float4(v.pos, 0.0, 1.0);
    o.uv = v.uv;
    o.col = v.col;
    return o;
}