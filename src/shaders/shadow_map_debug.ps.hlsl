struct PS_INPUIT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D shadowMap : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

const float NEAR_Z = 0.05f;
const float FAR_Z = 100.0f;


float4 main(PS_INPUIT input) : SV_TARGET
{
    float depth = shadowMap.Sample(samplerState, input.uv).r;
    //depth = (2.0 * NEAR_Z) / (FAR_Z + NEAR_Z - depth * (FAR_Z - NEAR_Z));

    return float4(depth, depth, depth, 1.f);
}
