struct VS_INPUT
{
    float3 pos : POSITION;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 localPos : POSITION0;
};

cbuffer transform : register(b0)
{
    row_major matrix view;
    row_major matrix projection;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(float4(input.pos, 0.0f), mul(view, projection)).xyzz;
    output.localPos = input.pos;
    
    return output;
}