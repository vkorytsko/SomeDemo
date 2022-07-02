struct PS_INPUIT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D tex : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

float4 main(PS_INPUIT input) : SV_TARGET
{
    return tex.Sample(samplerState, input.uv);
}
