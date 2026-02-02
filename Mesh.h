#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include <DirectXMath.h>
#include "Graphics.h"

class Mesh
{
private:
	/// <summary>
	/// Vertex Buffer
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>();

	/// <summary>
	/// Index Buffer
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>();
	
	/// <summary>
	/// Index Buffer Count
	/// </summary>
	int indexBufferCount;

	/// <summary>
	/// Vertex Buffer Count
	/// </summary>
	int vertexBufferCount;
public:
	Mesh(Vertex vertices[], int indices[], int verticesLength, int indiciesLength);
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	void Draw();
};

