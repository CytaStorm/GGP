#include "ShaderIncludes.hlsli"

cbuffer BufferStruct : register(b0)
{
    matrix view;
    matrix projection;
}

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

VertexToPixel_Sky main( VertexShaderInput input )
{
    VertexToPixel_Sky output;
    matrix editedMatrix = view;
    editedMatrix._14 = 0;
    editedMatrix._24 = 0;
    editedMatrix._34 = 0;

    output.position = mul(mul(projection, view), float4(input.localPosition, 1));
    output.position.z = output.position.w;
    output.sampleDir = input.localPosition;

    return output;
}