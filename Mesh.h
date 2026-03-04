#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include <DirectXMath.h>
#include "Graphics.h"
#include <vector>

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>();
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>();
	
	int m_indexBufferCount;
	int m_vertexBufferCount;

	void CreateVertexBuffer(int a_vertexCount, const Vertex* a_pFirstVertex);
	void CreateIndexBuffer(int a_indexCount, const UINT* a_pFirstIndex);


public:
	Mesh(Vertex a_vertices[], UINT a_indices[], int a_verticesLength, int a_indiciesLength);
	Mesh(const char* a_fileName);
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	void Draw();
};

