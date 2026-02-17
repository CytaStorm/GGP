#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> a_meshPtr)
{
	m_transform = Transform();
	m_mesh = a_meshPtr;

	//calculate geometric center of points
	DirectX::XMVECTOR center;
}

GameEntity::~GameEntity()
{

}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return m_mesh;
}

Transform& GameEntity::GetTransform()
{
	return m_transform;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> a_VSConstantBuffer)
{
	//update struct matrix;
	m_transform.CalculateWorldMatrix();
	m_vsData.m_world = m_transform.GetWorldMatrix();

	//memcpy shader
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(a_VSConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &m_vsData, sizeof(m_vsData));

	Graphics::Context->Unmap(a_VSConstantBuffer.Get(), 0);

	m_mesh->Draw();
}

void GameEntity::ChangeTint(DirectX::XMFLOAT4 a_newTint)
{
	m_vsData.m_colorTint = a_newTint;
}
