#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include "Light.h"
#include <string>
#include "Graphics.h"

struct Helper
{
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LoadTexture(const wchar_t* a_fileName);

	static DirectX::XMFLOAT4X4 CalculateNewLightViewMatrix(DirectX::XMFLOAT3 a_lightDirection, DirectX::XMFLOAT4X4 a_lightViewMatrix);

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	// --------------------------------------------------------
	// Loads shaders from compiled shader object (.cso) files
	// and also created the Input Layout that describes our 
	// vertex data to the rendering pipeline. 
	// - Input Layout creation is done here because it must 
	//    be verified against vertex shader byte code
	// - We'll have that byte code already loaded below
	// --------------------------------------------------------
	static void LoadVertexShader(
		Microsoft::WRL::ComPtr<ID3D11InputLayout>& a_pInputLayout,
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader,
		//Microsoft::WRL::ComPtr<ID3D11Buffer>& a_pVertexShaderConstantBuffer,
		const std::wstring a_fileName);

	static ID3DBlob* LoadVertexShaderInternal(
		const std::wstring& a_fileName,
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader);

	static void LoadPixelShader(
		const std::wstring& a_fileName,
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader);
};
