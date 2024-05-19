struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION0;
};

TextureCube radianceTexture : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);


static const float PI = 3.14159265359;


float4 main(PS_INPUT input) : SV_TARGET
{
    // the sample direction equals the hemisphere's orientation 
    float3 N = normalize(input.worldPos);
  
    float3 irradiance = float3(0.0f, 0.0f, 0.0f);

    float3 up = float3(0.0, 1.0, 0.0);
	float3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
			// spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += radianceTexture.Sample(samplerState, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    return float4(irradiance, 1.0f);
}
