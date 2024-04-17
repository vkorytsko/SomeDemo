struct PS_INPUIT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D tex : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

float4 main(PS_INPUIT input) : SV_TARGET
{
    float4 color = tex.Sample(samplerState, input.uv);
    clip(color.a < 0.1f ? -1 : 1);

    return color;
}
