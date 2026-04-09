#include "ShaderIncludes.hlsli"
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

Texture2D SurfaceTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MaskTexture: register(t2);

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
    float3 ambientColor;
}


float3 Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
    return att * att;
}

float3 CalculatePhong(VertexToPixel input, Light light, float3 surfaceColor)
{
 float3 refl = reflect(-light.direction, input.normal);

    float RdotV = saturate(dot(refl, normalize(input.worldPosition - cameraPosition)));
    return pow(RdotV, 256) *
       light.color *
       light.intensity *
       surfaceColor.xyz;
}

float3 PointLight(VertexToPixel input, Light light, float3 surfaceColor)
{
    //diffuse
    //float3 directionToLight = -lights[lightIndex].direction;
    float3 directionToLight = light.position - input.worldPosition;
    float3 diffuseTerm = saturate(dot(input.normal, directionToLight)) * 
        light.color * 
        light.intensity * 
        surfaceColor;  

    //phong
    float3 specularTerm = CalculatePhong(input, light, surfaceColor);

    specularTerm *= any(diffuseTerm);

    return (diffuseTerm + specularTerm) * Attenuate(light, input.worldPosition);
}

float3 DirectionalLight(VertexToPixel input, Light light, float3 surfaceColor)
{
    //diffuse
    float3 directionToLight = -light.direction;
    float3 diffuseTerm = saturate(dot(input.normal, directionToLight)) * 
        light.color * 
        light.intensity * 
        surfaceColor;  

    //phong
    float3 specularTerm = CalculatePhong(input, light, surfaceColor);
    specularTerm *= any(diffuseTerm);

    return diffuseTerm + specularTerm;
}

float3 SpotLight(VertexToPixel input, Light light, float3 surfaceColor)
{
    float lightToPixel = light.position - input.worldPosition;
    float pixelAngle = saturate(dot(lightToPixel, light.direction));

    float cosOuter = cos(light.spotOuterAngle);
    float cosInner = cos(light.spotInnerAngle);
    float fallOffRange = cosOuter - cosInner;

    float spotTerm = saturate((cosOuter - pixelAngle) / fallOffRange);

    return PointLight(input, light, surfaceColor) * spotTerm;
}

float3 CalculateNormals(VertexToPixel input)
{
    //corrected normal
    float4 normalFromTexture = NormalTexture.Sample(BasicSampler, input.uv);
    float3 unpackedNormal = normalize(normalFromTexture * 2.0f - 1.0f);

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
    input.uv = input.uv * scale + offset;
    //masking
    float4 surfaceColor =
        SurfaceTexture.Sample(BasicSampler, input.uv) *
        colorTint; //* MaskTexture.Sample(BasicSampler, input.uv);

    //ambient
    float3 ambientTerm = ambientColor * surfaceColor.xyz;

    //normals
    input.normal = CalculateNormals(input);
    float3 finalColor;

    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                finalColor += DirectionalLight(input, lights[i], surfaceColor.xyz);
                break;
            case LIGHT_TYPE_POINT:
                finalColor += PointLight(input, lights[i], surfaceColor.xyz);
                break;
            case LIGHT_TYPE_SPOT:
                finalColor += SpotLight(input, lights[i], surfaceColor.xyz);
                break;
        }
    }

    return float4(finalColor + ambientTerm, 1);

}