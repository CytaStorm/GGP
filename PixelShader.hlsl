#include "ShaderIncludes.hlsli"
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

Texture2D AlbedoTexture : register(t0);
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);

SamplerState BasicSampler : register(s0);

struct Light
{
    int type;
    float3 direction;
    float range;
    float3 position;
    float intensity;
    float3 color;
    float spotInnerAngle;
    float spotOuterAngle;
    float2 padding;
};

cbuffer PixelcBuffer : register(b0)
{
    float4 colorTint; //16
    float2 scale;
    float2 offset; // 16
    float3 cameraPosition;
    float timeElapsedMs; //16
    Light lights[5]; //16
}


float3 Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
    return att * att;
}

float D_GGX(float3 normal, float3 halfAngle, float roughness)
{
    float MIN_ROUGHNESS = 0.0000001;
    float NdotH = saturate(dot(normal, halfAngle));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS);
    
    float denomToSquare = NdotH2 * (a2 - 1) + 1;

    return a2 / (3.14 * denomToSquare * denomToSquare);
}

float G_SchlickGGX(float3 normal, float3 view, float roughness)
{
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(normal, view));

    return 1 / (NdotV * (1 - k) + k);

}

float3 F_Schlick(float3 view, float3 halfAngle, float3 specularColor)
{
    float VdotH = saturate(dot(view, halfAngle));

    return specularColor + (1 - specularColor) * pow(1 - VdotH, 5);
}

float3 DiffuseEnergyConserve(float3 diffuse, float3 fresnel, float metalness)
{ 
    return diffuse * (1 - fresnel) * (1 - metalness);
}

float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0)
{
    float3 halfAngle = normalize(n + l);
    float D = D_GGX(n, halfAngle, roughness);
    float G = G_SchlickGGX(n, v, roughness);
    float3 F = F_Schlick(v, halfAngle, f0);

    float3 specularResult = (D * F * G) / 4;
    return specularResult * saturate(dot(n, 1));
}

float3 CalculatePhong(VertexToPixel input, Light light, float3 albedoColor)
{
 float3 refl = reflect(-light.direction, input.normal);

    float RdotV = saturate(dot(refl, normalize(input.worldPosition - cameraPosition)));
    return pow(RdotV, 256) *
       light.color *
       light.intensity *
       albedoColor.xyz;
}

float3 PointLight(VertexToPixel input, Light light, float3 albedoColor, float roughness, float3 specularColor, float metalness)
{
    //diffuse
    //float3 directionToLight = -lights[lightIndex].direction;
    float3 directionToLight = light.position - input.worldPosition;
    float3 diffuseTerm = saturate(dot(input.normal, directionToLight)) * 
        light.color * 
        light.intensity * 
        albedoColor;  

    //specularlity
    float3 directionToCamera = cameraPosition - input.worldPosition;
    float3 specularTerm = MicrofacetBRDF(
        input.normal, directionToLight, directionToCamera, roughness, specularColor);

    float3 fresnel = F_Schlick(input.normal, input.normal + directionToLight, specularColor);
    float3 balancedDiff = DiffuseEnergyConserve(diffuseTerm, fresnel, metalness);

    float3 total = (balancedDiff * diffuseTerm + specularTerm) * light.intensity * light.color;

    return total * Attenuate(light, input.worldPosition);
}

float3 DirectionalLight(VertexToPixel input, Light light, float3 albedoColor, float roughness, float3 specularColor, float metalness)
{
    //diffuse
    float3 directionToLight = -light.direction;
    float3 diffuseTerm = saturate(dot(input.normal, directionToLight)) * 
        light.color * 
        light.intensity * 
        albedoColor;  

    //specularity
    float3 directionToCamera = cameraPosition - input.worldPosition;
    float3 specularTerm = MicrofacetBRDF(
        input.normal, directionToLight, directionToCamera, roughness, specularColor);

    float3 fresnel = F_Schlick(input.normal, input.normal + directionToLight, specularColor);
    float3 balancedDiff = DiffuseEnergyConserve(diffuseTerm, fresnel, metalness);

    float3 total = (balancedDiff * diffuseTerm + specularTerm) * light.intensity * light.color;

    return total;
}

float3 SpotLight(VertexToPixel input, Light light, float3 albedoColor, float roughness, float3 specularColor, float metalness)
{
    float3 lightToPixel = light.position - input.worldPosition;
    float pixelAngle = saturate(dot(lightToPixel, light.direction));

    float cosOuter = cos(light.spotOuterAngle);
    float cosInner = cos(light.spotInnerAngle);
    float fallOffRange = cosOuter - cosInner;

    float spotTerm = saturate((cosOuter - pixelAngle) / fallOffRange);

    return PointLight(input, light, albedoColor, roughness, specularColor, metalness) * spotTerm;
}

float3 CalculateNormals(VertexToPixel input)
{
    //corrected normal
    float4 normalFromTexture = NormalMap.Sample(BasicSampler, input.uv);
    float3 unpackedNormal = normalize(normalFromTexture * 2.0f - 1.0f).xyz;

    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent - N * dot(input.tangent, N));
    float3 B = cross(T, N);

    float3x3 TBN = float3x3(T, B, N);
    
    return mul(unpackedNormal, TBN);
}
// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    float4 albedoColor =
        pow(AlbedoTexture.Sample(BasicSampler, input.uv), 2.2) *
        colorTint; //* MaskTexture.Sample(BasicSampler, input.uv);

    //float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    //float metallic = MetalnessMap.Sample(BasicSampler, input.uv).r;
    float roughness = 0.5;
    float metallic = 0;
    float3 specularColor = lerp(0.04f, albedoColor.rgb, metallic);


    input.uv = input.uv * scale + offset;
    //masking

    //normals
    input.normal = CalculateNormals(input);
    float3 finalColor;

    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                finalColor += DirectionalLight(input, lights[i], albedoColor.xyz, roughness, specularColor, metallic);
                break;
            case LIGHT_TYPE_POINT:
                finalColor += PointLight(input, lights[i], albedoColor.xyz, roughness, specularColor, metallic);
                break;
            case LIGHT_TYPE_SPOT:
                finalColor += SpotLight(input, lights[i], albedoColor.xyz, roughness, specularColor, metallic);
                break;
        }
    }

    return pow(float4(finalColor, 1), (1.0 / 2.2));

}