struct PS_INPUIT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D shadowMap : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);


float4 main(PS_INPUIT input) : SV_TARGET
{
    float depth = shadowMap.Sample(samplerState, input.uv).r;
    depth = depth * 2.0 - 1.0; // Back to NDC

    return float4(depth, depth, depth, 1.f);
}
