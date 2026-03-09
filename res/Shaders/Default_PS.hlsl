struct PointLight
{
    float3 pos;
    float range;
    float3 color;
    float intensity;
};

cbuffer FrameCB : register(b1)
{
    float4x4 gViewProj;
    float3 gCameraPos;
    uint pad0;

    PointLight gLight0;
    PointLight gLight1;
};

Texture2D gTex0 : register(t0);
SamplerState gSamp0 : register(s0);

struct VertexOut
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 worldNrm : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

float3 EvalPointLight(PointLight Lgt, float3 P, float3 N)
{
    float3 toL = Lgt.pos - P;
    float d = length(toL);
    if (d >= Lgt.range)
        return 0.0f;

    float3 L = toL / max(d, 1e-4f);
    float NdotL = saturate(dot(N, L));

    float x = saturate(1.0f - d / Lgt.range);
    float att = x * x;

    float3 radiance = Lgt.color * Lgt.intensity;
    return radiance * (NdotL * att);
}

float4 PSMain(VertexOut i) : SV_TARGET
{
    float3 albedo = gTex0.Sample(gSamp0, i.uv).rgb;
    float3 N = normalize(i.worldNrm);

    float3 ambient = 0.05f * albedo;

    float3 lighting = 0.0f;
    lighting += EvalPointLight(gLight0, i.worldPos, N);
    lighting += EvalPointLight(gLight1, i.worldPos, N);

    float3 color = ambient + albedo * lighting;
    return float4(color, 1.0f);
}