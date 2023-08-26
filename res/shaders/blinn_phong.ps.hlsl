struct PS_INPUIT
{
    float4 pos : SV_POSITION;
    float4 fragPos : FRAGPOS_WS;
    float4 fragPosLightSpace : FRAGPOS_LS;
    float3 viewPos : VIEWPOS;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

cbuffer material : register(b0)
{
    float mShiness;
};

cbuffer posLight : register(b1)
{
    float3 plPosition;
    float3 plAmbient;
    float3 plDiffuse;
    float3 plSpecular;
    float3 plAttenuation;  // (x: constant, y: linear, z: quadratic)
};

cbuffer dirLight : register(b2)
{
    float3 dlDirection;
    float3 dlAmbient;
    float3 dlDiffuse;
    float3 dlSpecular;
};

cbuffer spotLight : register(b3)
{
    float3 slPosition;
    float3 slDirection;
    float3 slAmbient;
    float3 slDiffuse;
    float3 slSpecular;
    float3 slAttenuation;  // (x: constant, y: linear, z: quadratic)
    float2 slCutOff;  // (x: inner, y: outer)
    int slEnabled;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
Texture2D specularTexture : TEXTURE : register(t1);
Texture2D shadowMap : TEXTURE : register(t2);
SamplerState samplerState : SAMPLER : register(s0);


float3 calcDirLight(float3 normal, float3 viewDir, float2 uv);
float3 calcPointLight(float3 normal, float3 viewDir, float2 uv, float3 fragPos, float3 fragPosLightSpace);
float3 calcSpotLight(float3 normal, float3 viewDir, float2 uv, float3 fragPos);
float calcShadow(float3 fragPosLightSpace);


float4 main(PS_INPUIT input) : SV_TARGET
{
    float alpha = diffuseTexture.Sample(samplerState, input.uv).a;
	clip(alpha < 0.1 ? -1 : 1);

    float3 fragPos = input.fragPos.xyz / input.fragPos.w;
    float3 fragPosLightSpace = input.fragPosLightSpace.xyz / input.fragPosLightSpace.w;
    
    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(input.viewPos - fragPos);

    float3 color = calcDirLight(normal, viewDir, input.uv);
    color += calcPointLight(normal, viewDir, input.uv, fragPos, fragPosLightSpace);
    if (slEnabled > 0)
    {
        color += calcSpotLight(normal, viewDir, input.uv, fragPos);
    }
    
    return float4(color, alpha);
}

float3 calcDirLight(float3 normal, float3 viewDir, float2 uv)
{
    float3 lightDir = normalize(dlDirection);
    float3 halfwayDir = normalize(lightDir + viewDir);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // specular
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), mShiness);

    // results
    float3 ambient = dlAmbient * (float3) diffuseTexture.Sample(samplerState, uv);
    float3 diffuse = dlDiffuse * diff * (float3) diffuseTexture.Sample(samplerState, uv);
    float3 specular = dlSpecular * spec * (float3) specularTexture.Sample(samplerState, uv);
    
    float3 color = ambient + diffuse + specular;

    return color;
}


float3 calcPointLight(float3 normal, float3 viewDir, float2 uv, float3 fragPos, float3 fragPosLightSpace)
{
    float3 lightDir = normalize(plPosition - fragPos);
    float3 halfwayDir = normalize(lightDir + viewDir);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), mShiness);
    
    // attenuation
    float distance = length(plPosition - fragPos);
    float attenuation = 1.0 / (plAttenuation.x + plAttenuation.y * distance + plAttenuation.z * (distance * distance));
    
    // shadow
    float shadow = 1.0f - calcShadow(fragPosLightSpace);
    
    // results
    float3 ambient = plAmbient * (float3) diffuseTexture.Sample(samplerState, uv) * attenuation;
    float3 diffuse = plDiffuse * diff * (float3) diffuseTexture.Sample(samplerState, uv) * attenuation * shadow;
    float3 specular = plSpecular * spec * (float3) specularTexture.Sample(samplerState, uv) * attenuation * shadow;

    float3 color = ambient + diffuse + specular;
    
    return color;
}

float3 calcSpotLight(float3 normal, float3 viewDir, float2 uv, float3 fragPos)
{
    float3 lightDir = normalize(slPosition - fragPos);
    float3 halfwayDir = normalize(lightDir + viewDir);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // specular
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), mShiness);
 
    // attenuation
    float distance = length(slPosition - fragPos);
    float attenuation = 1.0 / (slAttenuation.x + slAttenuation.y * distance + slAttenuation.z * (distance * distance));

    // intensity
    float theta = dot(lightDir, normalize(-slDirection));
    float epsilon = slCutOff.x - slCutOff.y;
    float intensity = clamp((theta - slCutOff.y) / epsilon, 0.0f, 1.0f);

    // results
    float3 ambient = slAmbient * (float3) diffuseTexture.Sample(samplerState, uv) * attenuation * intensity;
    float3 diffuse = slDiffuse * diff * (float3) diffuseTexture.Sample(samplerState, uv) * attenuation * intensity;
    float3 specular = slSpecular * spec * (float3) specularTexture.Sample(samplerState, uv) * attenuation * intensity;
    
    float3 color = ambient + diffuse + specular;

    return color;
}

float calcShadow(float3 fragPosLightSpace)
{
    float bias = 0.0005f;
    fragPosLightSpace.z -= bias;
    if ((fragPosLightSpace.x > 1.0f || fragPosLightSpace.x < -1.0f) || (fragPosLightSpace.y > 1.0f || fragPosLightSpace.y < -1.0f) || (fragPosLightSpace.z > 1.0f || fragPosLightSpace.z < 0.0f))
    {
        return 0.0f;
    }
    
    // back to NDC and flip y for texture mapping
    float2 shadowDepthUV = fragPosLightSpace.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    float shadowDepth = shadowMap.Sample(samplerState, shadowDepthUV).r;
    float fragDepth = fragPosLightSpace.z;

    return shadowDepth > fragDepth ? 0.0 : 1.0;
}


