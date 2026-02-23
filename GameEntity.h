#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>
#include <wrl/client.h>
#include "BufferStructs.h"
#include "Camera.h"

class GameEntity
{
private:
	BufferStructs m_vsData = BufferStructs();

public:
	Transform m_transform;
	std::shared_ptr<Mesh> m_mesh;

	GameEntity(std::shared_ptr<Mesh> a_meshPtr);
	~GameEntity();

	std::shared_ptr<Mesh> GetMesh();
	Transform& GetTransform();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> a_VSConstantBuffer, std::shared_ptr<Camera> a_camera);
	void ChangeTint(DirectX::XMFLOAT4 a_newTint);
};

