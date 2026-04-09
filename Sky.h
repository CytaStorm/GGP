#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include <DirectXMath.h>
#include <memory>
#include "mesh.h";

class Sky
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencil;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
	std::shared_ptr<Mesh> m_pMesh;

public:
	Sky(
		std::shared_ptr<Mesh> a_pMesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> a_pSamplerState,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> a_pSRV);
	~Sky();
};

