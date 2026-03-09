cbuffer UICB : register(b0)
{
    float4 gTint; // rgba, optionnel (1,1,1,1)
};

Texture2D gTex0 : register(t0);

SamplerState gSamp0 : register(s0);

struct VertexIn
{
    float2 pos : POSITION; // [-1..1]
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
};

struct VertexOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR0;
};


float4 PSMain(VertexOut i) : SV_TARGET
{
    float4 tex = gTex0.Sample(gSamp0, i.uv);
    return tex * i.col * gTint; // * i.col; // optionnel
}