#include "Helper.h"
#include <d3dcompiler.h>
#include "PathHelpers.h"
#include "Graphics.h"

void Helper::LoadVertexShader(
	ID3DBlob** a_pVertexShaderBlob, Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader)
{
	D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), a_pVertexShaderBlob);

	Graphics::Device->CreateVertexShader(
		(*a_pVertexShaderBlob)->GetBufferPointer(), // Pointer to start of binary data
		(*a_pVertexShaderBlob)->GetBufferSize(),// How big is that data?
		0,// No classes in this shader
		a_pVertexShader.GetAddressOf());// ID3D11VertexShader**
}

void Helper::LoadPixelShader(
	ID3DBlob** a_pPixelShaderBlob, Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader)
{
	D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), a_pPixelShaderBlob);

	Graphics::Device->CreatePixelShader(
		(*a_pPixelShaderBlob)->GetBufferPointer(), // Pointer to start of binary data
		(*a_pPixelShaderBlob)->GetBufferSize(),// How big is that data?
		0,// No classes in this shader
		a_pPixelShader.GetAddressOf());// ID3D11VertexShader**
}
