struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 localPos : POSITION0;
};

TextureCube environmentTexture : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);


float4 main(PS_INPUT input) : SV_TARGET
{
    float3 color = environmentTexture.Sample(samplerState, input.localPos).rgb;

    // HDR tonemap and gamma correct
    color = color / (color + float3(1.0f, 1.0f, 1.0f));
    color = pow(color, 1.0 / 2.2);

    return float4(color, 1.0f);
}
