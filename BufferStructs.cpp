#include "BufferStructs.h"

BufferStructs::BufferStructs()
{
	//Identity transform matrix
	DirectX::XMStoreFloat4x4(&m_world, DirectX::XMMatrixIdentity());
	// white tint, original color shown
	m_colorTint = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}
