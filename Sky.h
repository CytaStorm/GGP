#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include <DirectXMath.h>
#include <memory>
#include "mesh.h"
#include "Camera.h"
#include "Material.h"
#include "BufferStructs.h"

class Sky
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencil;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
	std::shared_ptr<Material> m_pMaterial;
	std::shared_ptr<Mesh> m_pMesh;

	//VS buffer
	SkyVSConstantBuffer m_skyBuffer = SkyVSConstantBuffer();

	// --- HEADER ---
	
	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
public:
	Sky();
	Sky(
		std::shared_ptr<Mesh> a_pMesh,
		const wchar_t* a_skyTextureFront,
		const wchar_t* a_skyTextureBack,
		const wchar_t* a_skyTextureLeft,
		const wchar_t* a_skyTextureRight,
		const wchar_t* a_skyTextureUp,
		const wchar_t* a_skyTextureDown,
		std::wstring a_skyVertexShaderFilePath,
		std::wstring a_skyPixelShaderFilePath,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> a_pSamplerState);
	~Sky();
	void Draw(std::shared_ptr<Camera> a_pCamera);
};

