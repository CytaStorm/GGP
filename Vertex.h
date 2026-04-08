#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 m_position;	    // The local position of the vertex
	DirectX::XMFLOAT2 m_uv;
	DirectX::XMFLOAT3 m_normal;
	DirectX::XMFLOAT3 m_tangent;
};