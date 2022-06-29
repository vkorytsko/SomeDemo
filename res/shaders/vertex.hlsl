struct VS_Out
{
    float4 color : Color;
    float4 pos : SV_Position;
};

VS_Out main(float2 pos : Position, float4 color : Color)
{
    VS_Out vso;
    vso.pos = float4(pos, 0.0f, 1.0f);
    vso.color = color;
    
    return vso;
}