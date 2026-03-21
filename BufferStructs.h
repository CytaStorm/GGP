#pragma once
#include <DirectXMath.h>

struct VertexShaderConstantBuffer
{
	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;

	VertexShaderConstantBuffer();
};

struct PSConstantBuffer {
	DirectX::XMFLOAT4 m_colorTint;
	DirectX::XMFLOAT2 m_scale;
	DirectX::XMFLOAT2 m_offset;
	float m_timeElapsedMs;
	PSConstantBuffer();
};
