#include "Sky.h"
#include "WICTextureLoader.h"
#include "Material.h"
#include "Game.h"

Sky::Sky(
	std::shared_ptr<Mesh> a_pMesh, 
	const wchar_t* a_skyTextureFilePath,
	std::wstring a_skyVertexShaderFilePath,
	std::wstring a_skyPixelShaderFilePath,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> a_pSamplerState)
{
	m_pMesh = a_pMesh;
	m_pSampler = a_pSamplerState;

	//load shaders
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVSConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pPSConstantBuffer;

	//vert
	Game::LoadVertexShader(
		pInputLayout,
		m_pVertexShader,
		pVSConstantBuffer,
		a_skyVertexShaderFilePath
	);
	
	//pix
	Game::LoadPixelShader<PSConstantBuffer>(
		PSConstantBuffer(),
		m_pPixelShader,
		pPSConstantBuffer,
		a_skyPixelShaderFilePath
	);
	

	//diffuse
	DirectX::CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		a_skyTextureFilePath,
		0,
		m_pSRV.GetAddressOf()
	);

	std::shared_ptr<Material> pSkyboxMaterial = std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		m_pVertexShader,	//vert shader
		m_pPixelShader		//pix shader
	);

	pSkyboxMaterial->AddTextureSRV(0, m_pSRV);
	pSkyboxMaterial->AddSampler(0, a_pSamplerState);


	//Rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;

	Graphics::Device.Get()->CreateRasterizerState(&rasterizerDesc, &m_pRasterizer);

	//Depth stencil
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	Graphics::Device.Get()->CreateDepthStencilState(&depthDesc, &m_pDepthStencil);

}

Sky::~Sky()
{
}
