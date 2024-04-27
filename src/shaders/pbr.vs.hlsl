struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 fragPos : FRAGPOS_WS;
    float3 viewPos : VIEWPOS;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
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
    float4 posWS = mul(float4(input.position, 1.0f), model);
    float4 normalWS = mul(float4(input.normal, 0.0f), model);
    float4 tangentWS = mul(float4(input.tangent.xyz * input.tangent.w, 0.0f), model);
    //float4 tangentWS = mul(float4(input.tangent.xyz * 1.0, 0.0f), model);

    output.pos = mul(posWS, mul(view, projection));
    output.fragPos = posWS.xyz;
    output.viewPos = viewPos;
    output.normal = normalize(normalWS.xyz);
    output.tangent = normalize(tangentWS.xyz);
    output.uv = input.uv;
    
    return output;
}
