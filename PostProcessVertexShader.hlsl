#include "ShaderIncludes.hlsli"

VertexToPixel main( uint id : SV_VertexID )
{
    VertexToPixel output;
    // Calculate the UV (0,0) to (2,2) using the ID
    output.uv = float2(
        (id << 1) & 2, // Essentially: id % 2 * 2
        id & 2);

    // Calculate the position based on the UV
    output.screenPosition = float4(output.uv, 0, 1);
    output.screenPosition.x = output.screenPosition.x * 2 - 1;
    output.screenPosition.y = output.screenPosition.y * -2 + 1;

    return output;
}