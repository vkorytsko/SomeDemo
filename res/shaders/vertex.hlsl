struct VS_Out
{
    float4 color : Color;
    float4 pos : SV_Position;
};

cbuffer CB
{
    matrix transform;
};

VS_Out main(float3 pos : Position, float4 color : Color)
{
    VS_Out vso;
    vso.pos = mul(float4(pos, 1.0f), transform);
    vso.color = color;
    
    return vso;
}