#include "BufferStructs.h"

VSConstantBuffer::VSConstantBuffer()
{
	//Identity transform matrix
	DirectX::XMStoreFloat4x4(&m_worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_worldInverseTranspose, DirectX::XMMatrixIdentity());
	// white tint, original color shown
	DirectX::XMStoreFloat4x4(&m_lightViewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_lightProjectionMatrix, DirectX::XMMatrixIdentity());
}

PSConstantBuffer::PSConstantBuffer()
{
	m_colorTint = { 1.0f, 1.0f, 1.0f, 1.0f }; //white;
	m_scale = { 1, 1 };
	m_offset = { 0, 0 };
	m_timeElapsedMs = 0;
	m_cameraPosition = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 5; i++) {
		m_lights[i] = Light{};
	}
	DirectX::XMStoreFloat4x4(&m_lightViewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_lightProjectionMatrix, DirectX::XMMatrixIdentity());
}

SkyVSConstantBuffer::SkyVSConstantBuffer()
{
	DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixIdentity());
}

SkyVSConstantBuffer::SkyVSConstantBuffer(DirectX::XMFLOAT4X4 a_projectionMatrix, DirectX::XMFLOAT4X4 a_viewMatrix)
{
	m_projectionMatrix = a_projectionMatrix;
	m_viewMatrix = a_viewMatrix;
}

ShadowVSData::ShadowVSData()
{
	DirectX::XMStoreFloat4x4(&m_proj, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_view, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_world, DirectX::XMMatrixIdentity());
}

GaussianBlurPostProcessConstantBuffer::GaussianBlurPostProcessConstantBuffer()
{
	m_blurAmount = 0;
}
