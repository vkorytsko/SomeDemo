struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer transform : register(b0)
{
    row_major matrix model;
    row_major matrix view;
    row_major matrix projection;
    float3 viewPos;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(float4(input.pos, 1.0f), mul(model, mul(view, projection)));
    output.uv = input.uv;
    
    return output;
}

