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
	
	UINT m_indexBufferCount;
	UINT m_vertexBufferCount;

	void CreateVertexBuffer(UINT ta_vertexCount, const Vertex* a_pFirstVertex);
	void CreateIndexBuffer(UINT a_indexCount, const UINT* a_pFirstIndex);

	void CalculateTangents(Vertex* a_verts, int a_numVerts, unsigned int* a_indicies, int a_numIndicies);

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

