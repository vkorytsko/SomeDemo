struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 fragPos : FRAGPOS_WS;
    float4 fragPosLightSpace : FRAGPOS_LS;
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

cbuffer posLightTransform : register(b1)
{
    row_major matrix posLightView;
    row_major matrix posLightProj;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 posWS = mul(float4(input.pos, 1.0f), model);
    float4 fragPosLightSpace = mul(posWS, mul(posLightView, posLightProj));

    output.pos = mul(posWS, mul(view, projection));
    output.fragPos = posWS;
    output.fragPosLightSpace = fragPosLightSpace;
    output.viewPos = viewPos;
    output.normal = input.normal;
    output.uv = input.uv;
    
    return output;
}
