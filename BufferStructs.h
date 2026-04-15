#pragma once
#include <DirectXMath.h>
#include "Light.h"
#include <array>

struct VertexShaderConstantBuffer
{
	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_worldInverseTranspose;

	VertexShaderConstantBuffer();
};

struct PSConstantBuffer {
	DirectX::XMFLOAT4 m_colorTint; // 16
	DirectX::XMFLOAT2 m_scale;
	DirectX::XMFLOAT2 m_offset; // 16
	DirectX::XMFLOAT3 m_cameraPosition;
	float m_timeElapsedMs; // 16
	std::array<Light, 5> m_lights;

	PSConstantBuffer();
};

struct SkyVSConstantBuffer {
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;

	SkyVSConstantBuffer();
	SkyVSConstantBuffer(DirectX::XMFLOAT4X4 a_projectionMatrix, DirectX::XMFLOAT4X4 a_viewMatrix);
};
