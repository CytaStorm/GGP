#pragma once
#include <DirectXMath.h>
class BufferStructs
{
public:
	DirectX::XMFLOAT4 m_colorTint;
	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;

	BufferStructs();
};

