static const float PI = 3.14159265f;

struct PS_INPUIT
{
    float4 position : SV_POSITION;
    float3 fragPos : FRAGPOS_WS;
    float3 viewPos : VIEWPOS;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

cbuffer material : register(b0)
{
    float4 baseColorFactor;
    float normalMapScale;
    float metallicFactor;
    float roughnessFactor;
};

cbuffer posLight : register(b1)
{
    float3 plPosition;
    float3 plColor;
};


Texture2D albedoMap : TEXTURE : register(t0);
Texture2D normalMap : TEXTURE : register(t1);
Texture2D metallicRoughnessMap : TEXTURE : register(t2);

SamplerState albedoSampler : SAMPLER : register(s0);
SamplerState normalSampler : SAMPLER : register(s1);
SamplerState metallicRoughnessSampler : SAMPLER : register(s2);


float3 getNormalFromMap(PS_INPUIT input);
float3 fresnelSchlick(float cosTheta, float3 F0);
float DistributionGGX(float3 N, float3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);


float4 main(PS_INPUIT input) : SV_TARGET
{
    float4 albedo = pow(albedoMap.Sample(albedoSampler, input.uv), 2.2f);
    albedo *= baseColorFactor;

    //clip(albedo.a < 0.1 ? -1 : 1);
    clip(albedo.a < 0.9 ? -1 : 1);

    const float3 normal = getNormalFromMap(input);
    float3 metallicRoughness = metallicRoughnessMap.Sample(metallicRoughnessSampler, input.uv);
    metallicRoughness *= float3(1.0f, roughnessFactor, metallicFactor);

    const float3 worldPos = input.fragPos;

    float3 N = normalize(input.normal);
    float3 V = normalize(input.viewPos - worldPos);

    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo.xyz, metallicRoughness.b);

    // reflectance equation
    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    {
        // calculate per-light radiance
        float3 L = normalize(plPosition - worldPos);
        float3 H = normalize(V + L);
        float distance = length(plPosition - worldPos);
        float attenuation = 1 / (distance * distance);
        float3 radiance = plColor * attenuation;

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, metallicRoughness.g);
        float G = GeometrySmith(N, V, L, metallicRoughness.g);
        float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        float3 kS = F;
        float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
        kD *= 1.0 - metallicRoughness.b;

        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        float3 specular = numerator / denominator;

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo.xyz / PI + specular) * radiance * NdotL;
    }
    

    float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo.xyz * 1.0f;
    float3 color = ambient + Lo;

    color = color / (color + float3(1.0f, 1.0f, 1.0f));
    color = pow(color, 1.0f / 2.2f);
    
    return float4(color, albedo.a);
    //return float4(normal.rgb * 0.5 + 0.5, 1.0f);
}

float3 getNormalFromMap(PS_INPUIT input)
{
    float3 normal = normalMap.Sample(normalSampler, input.uv).xyz * 2.0 - 1.0;
    normal *= float3(normalMapScale, normalMapScale, 1.0);

    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = -cross(N, T);

    float3x3 TBN = transpose(float3x3(T, B, N));

    return normalize(mul(TBN, normal));
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
