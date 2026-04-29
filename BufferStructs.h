#pragma once
#include <DirectXMath.h>
#include "Light.h"
#include <array>

struct VSConstantBuffer
{
	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_worldInverseTranspose;
	DirectX::XMFLOAT4X4 m_lightViewMatrix;
	DirectX::XMFLOAT4X4 m_lightProjectionMatrix;
	VSConstantBuffer();
};

struct PSConstantBuffer {
	DirectX::XMFLOAT4 m_colorTint; // 16
	DirectX::XMFLOAT2 m_scale;
	DirectX::XMFLOAT2 m_offset; // 16

	DirectX::XMFLOAT3 m_cameraPosition;
	float m_timeElapsedMs; // 16

	DirectX::XMFLOAT4X4 m_lightViewMatrix;
	DirectX::XMFLOAT4X4 m_lightProjectionMatrix;

	std::array<Light, 5> m_lights;

	PSConstantBuffer();
};

struct SkyVSConstantBuffer {
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;

	SkyVSConstantBuffer();
	SkyVSConstantBuffer(DirectX::XMFLOAT4X4 a_projectionMatrix, DirectX::XMFLOAT4X4 a_viewMatrix);
};

struct ShadowVSData
{
	DirectX::XMFLOAT4X4 m_world;
	DirectX::XMFLOAT4X4 m_view;
	DirectX::XMFLOAT4X4 m_proj;

	ShadowVSData();
};

struct GaussianBlurPostProcessConstantBufferHorizontal
{
	int m_blurRadius;
	float m_pixelWidth;
	
	GaussianBlurPostProcessConstantBufferHorizontal();
	GaussianBlurPostProcessConstantBufferHorizontal(int a_blurRadius, float a_pixelWidth);
};

struct GaussianBlurPostProcessConstantBufferVertical
{
	int m_blurRadius;
	float m_pixelHeight;
	
	GaussianBlurPostProcessConstantBufferVertical();
	GaussianBlurPostProcessConstantBufferVertical(int a_blurRadius, float a_pixelHeight);
};

