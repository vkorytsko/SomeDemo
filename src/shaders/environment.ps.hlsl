struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION0;
};

Texture2D environmentTexture : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);


static const float2 invAtan = float2(-0.1591f, -0.3183f);

float2 SampleSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    const float2 uv = SampleSphericalMap(normalize(input.worldPos)); // make sure to normalize localPos
    const float3 color = environmentTexture.Sample(samplerState, uv).rgb;

    return float4(color, 1.0f);
}
