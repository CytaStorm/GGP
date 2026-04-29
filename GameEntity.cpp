#include "GameEntity.h"
#include <wrl/client.h>
#include "BufferStructs.h"
#include "Camera.h"

GameEntity::GameEntity(
	std::shared_ptr<Mesh> a_pMesh,
	std::shared_ptr<Material> a_pMaterial
)
{
	m_lifetimeMs = 0.0f;
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

void GameEntity::Draw(
	//Microsoft::WRL::ComPtr<ID3D11Buffer> a_VSConstantBuffer,
	//Microsoft::WRL::ComPtr<ID3D11Buffer> a_PSConstantBuffer,
	std::shared_ptr<Camera> a_camera,
	DirectX::XMFLOAT4X4 a_lightViewMatrix,
	DirectX::XMFLOAT4X4 a_lightProjectionMatrix)
{

	Graphics::Context->VSSetShader(m_pMaterial->GetVertexShader().Get(), 0, 0);
	Graphics::Context->PSSetShader(m_pMaterial->GetPixelShader().Get(), 0, 0);

	//update struct matrix;

	//vertex shader
	m_transform.CalculateWorldMatrix();
	m_VSConstantBuffer.m_worldMatrix = m_transform.GetWorldMatrix();
	m_VSConstantBuffer.m_projectionMatrix = a_camera->GetProjectionMatrix();
	m_VSConstantBuffer.m_viewMatrix = a_camera->GetViewMatrix();
	m_VSConstantBuffer.m_worldInverseTranspose = m_transform.GetWorldInverseTransposeMatrix();

	m_VSConstantBuffer.m_lightProjectionMatrix = a_lightProjectionMatrix;
	m_VSConstantBuffer.m_lightViewMatrix = a_lightViewMatrix;


	//pixel shader buffer
	m_PSConstantBuffer.m_colorTint = m_pMaterial->GetColorTint();
	m_PSConstantBuffer.m_timeElapsedMs = m_lifetimeMs;
	m_PSConstantBuffer.m_scale = m_pMaterial->GetUVscale();
	m_PSConstantBuffer.m_offset = m_pMaterial->GetUVoffset();
	m_PSConstantBuffer.m_cameraPosition = a_camera->GetTransform().GetPosition();

	m_PSConstantBuffer.m_lightProjectionMatrix = a_lightProjectionMatrix;
	m_PSConstantBuffer.m_lightViewMatrix = a_lightViewMatrix;

	//memcpy shader
	//vertex shader buffer
	//D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	//Graphics::Context->Map(a_VSConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	//memcpy(mappedBuffer.pData, &m_VSConstantBuffer, sizeof(m_VSConstantBuffer));
	//Graphics::Context->Unmap(a_VSConstantBuffer.Get(), 0);

	////pixel shader buffer
	//Graphics::Context->Map(a_PSConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	//memcpy(mappedBuffer.pData, &m_PSConstantBuffer, sizeof(m_PSConstantBuffer));
	//Graphics::Context->Unmap(a_PSConstantBuffer.Get(), 0);

	Graphics::FillAndBindNextConstantBuffer(
		&m_VSConstantBuffer,
		sizeof(m_VSConstantBuffer),
		D3D11_VERTEX_SHADER,
		0);

	Graphics::FillAndBindNextConstantBuffer(
		&m_PSConstantBuffer,
		sizeof(m_PSConstantBuffer),
		D3D11_PIXEL_SHADER,
		0);

	m_pMesh->Draw();
}

void GameEntity::ShadowDraw(
	DirectX::XMFLOAT4X4 a_lightViewMatrix,
	DirectX::XMFLOAT4X4 a_lightProjectionMatrix,
	Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pShadowVertexShader
	)
{
	Graphics::Context->VSSetShader(a_pShadowVertexShader.Get(), 0, 0);

	//shadow vert shader
	ShadowVSData vsData = {};
	vsData.m_view = a_lightViewMatrix;
	vsData.m_proj = a_lightProjectionMatrix;
	vsData.m_world = m_transform.GetWorldMatrix();

	Graphics::FillAndBindNextConstantBuffer(&vsData, sizeof(ShadowVSData), D3D11_VERTEX_SHADER, 0);
	m_pMesh->Draw();
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return m_pMaterial;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> a_pMaterial)
{
	m_pMaterial = a_pMaterial;
}
