struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 fragPos : FRAGPOS;
    float3 viewPos : VIEWPOS;
    float3 normal : NORMAL;
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
    output.fragPos = mul(float4(input.pos, 1.0f), model).xyz;
    output.viewPos = viewPos;
    output.normal = input.normal;
    output.uv = input.uv;
    
    return output;
}
