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
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>();

	/// <summary>
	/// Index Buffer
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>();
	
	/// <summary>
	/// Index Buffer Count
	/// </summary>
	int m_indexBufferCount;

	/// <summary>
	/// Vertex Buffer Count
	/// </summary>
	int m_vertexBufferCount;
public:
	Mesh(Vertex a_vertices[], int a_indices[], int a_verticesLength, int a_indiciesLength);
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	void Draw();
};

