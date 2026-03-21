#include "BufferStructs.h"

VertexShaderConstantBuffer::VertexShaderConstantBuffer()
{
	//Identity transform matrix
	DirectX::XMStoreFloat4x4(&m_worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixIdentity());
	// white tint, original color shown
}

PSConstantBuffer::PSConstantBuffer()
{
	m_colorTint = { 1.0f, 1.0f, 1.0f, 1.0f }; //white;
	m_scale = { 1, 1 };
	m_offset = { 0, 0 };
	m_timeElapsedMs = 0;
}
