struct PS_INPUT
{
    float4 pos : SV_POSITION;
};

cbuffer posLight : register(b0)
{
    float3 color;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(color, 1.0f);
}
