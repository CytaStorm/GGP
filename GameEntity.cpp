#include "GameEntity.h"
#include <wrl/client.h>
#include "BufferStructs.h"
#include "Camera.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> a_pMesh, std::shared_ptr<Material> a_pMaterial)
{
	m_transform = Transform();
	m_pMesh = a_pMesh;
	m_pMaterial = a_pMaterial;
}

GameEntity::~GameEntity()
{

}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return m_pMesh;
}

Transform& GameEntity::GetTransform()
{
	return m_transform;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> a_VSConstantBuffer, std::shared_ptr<Camera> a_camera)
{

	Graphics::Context->VSSetShader(m_pMaterial->GetVertexShader().Get(), 0, 0);
	Graphics::Context->PSSetShader(m_pMaterial->GetPixelShader().Get(), 0, 0);
	m_vsData.m_colorTint = m_pMaterial->GetColor();
	//update struct matrix;
	m_transform.CalculateWorldMatrix();
	m_vsData.m_worldMatrix = m_transform.GetWorldMatrix();
	m_vsData.m_projectionMatrix = a_camera->GetProjectionMatrix();
	m_vsData.m_viewMatrix = a_camera->GetViewMatrix();

	//memcpy shader
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(a_VSConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &m_vsData, sizeof(m_vsData));

	Graphics::Context->Unmap(a_VSConstantBuffer.Get(), 0);

	m_pMesh->Draw();
}

void GameEntity::ChangeTint(DirectX::XMFLOAT4 a_newTint)
{
	m_vsData.m_colorTint = a_newTint;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return m_pMaterial;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> a_pMaterial)
{
	m_pMaterial = a_pMaterial;
}
