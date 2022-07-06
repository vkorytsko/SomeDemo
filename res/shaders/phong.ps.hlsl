struct PS_INPUIT
{
    float4 pos : SV_POSITION;
    float3 fragPos : FRAGPOS;
    float3 viewPos : VIEWPOS;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

cbuffer material : register(b0)
{
    float3 mAmbient;
    float3 mDiffuse;
    float3 mSpecular;
    float mShiness;
};

cbuffer light : register(b1)
{
    float3 lPosition;
 
    float3 lAmbient;
    float3 lDiffuse;
    float3 lSpecular;
};

Texture2D tex : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

float4 main(PS_INPUIT input) : SV_TARGET
{
    // ambient
    float3 ambient = lAmbient * mAmbient;
    
    // diffuse
    float3 normal = normalize(input.normal);
    float3 lightDir = normalize(lPosition - input.fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = lDiffuse * (diff * mDiffuse);
    
    // specular
    float3 viewDir = normalize(input.viewPos - input.fragPos);
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mShiness);
    float3 specular = lSpecular * (spec * mSpecular);

    float3 color = ambient + diffuse + specular;
    
    return float4(color, 1.0f);
}
