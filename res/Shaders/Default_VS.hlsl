cbuffer ObjectCB : register(b0)
{
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
    float4x4 gWorldViewProj;
};

struct VertexIn
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VertexOut
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 worldNrm : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

VertexOut VSMain(VertexIn v)
{
    VertexOut o;

    float4 wp = mul(float4(v.pos, 1.0f), gWorld);
    o.worldPos = wp.xyz;

    o.worldNrm = normalize(mul(float4(v.normal, 0.0f), gWorldInvTranspose).xyz);

    o.pos = mul(float4(v.pos, 1.0f), gWorldViewProj);
    o.uv = v.uv;

    return o;
}