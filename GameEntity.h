#pragma once
#include "Transform.h"
#include "Material.h"
#include "Camera.h"
#include "Mesh.h"
#include <memory>

class GameEntity
{
private:
	BufferStructs m_vsData = BufferStructs();
	Transform m_transform;
	std::shared_ptr<Mesh> m_pMesh;
	std::shared_ptr<Material> m_pMaterial;

public:

	GameEntity(std::shared_ptr<Mesh> a_pMesh, std::shared_ptr<Material> a_pMaterial);
	~GameEntity();

	std::shared_ptr<Mesh> GetMesh();
	Transform& GetTransform();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> a_VSConstantBuffer, std::shared_ptr<Camera> a_camera);
	void ChangeTint(DirectX::XMFLOAT4 a_newTint);

	std::shared_ptr<Material> GetMaterial();
	void SetMaterial(std::shared_ptr<Material> a_pMaterial);
};

