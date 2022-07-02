struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

cbuffer CB
{
    matrix transform;
};

VS_OUT main(float3 pos : POSITION, float4 color : COLOR)
{
    VS_OUT output;
    output.pos = mul(float4(pos, 1.0f), transform);
    output.color = color;
    
    return output;
}
