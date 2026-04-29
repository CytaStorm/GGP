#include "Helper.h"
#include "WICTextureLoader.h"
#include "PathHelpers.h"

#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Helper::LoadTexture(const wchar_t* a_fileName)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newSRV;
	DirectX::CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		a_fileName,
		0,
		newSRV.GetAddressOf()
	);
	
	return newSRV;
}

DirectX::XMFLOAT4X4 Helper::CalculateNewLightViewMatrix(DirectX::XMFLOAT3 a_lightDirection, DirectX::XMFLOAT4X4 a_lightViewMatrix)
{
	//edge case
	DirectX::XMVECTOR worldup = DirectX::XMVectorSet(0, 1, 0, 0);
	if (a_lightDirection.x == 0 && a_lightDirection.y == -1 && a_lightDirection.z == 0) {
		worldup = DirectX::XMVectorSet(0.0f, 0.0f, 0.4794255f, 0.8775826f);
		//worldup = DirectX::XMVectorSet(0, 0, 1, 0);
	}

	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&a_lightDirection);
	DirectX::XMVECTOR negativeDirection = DirectX::XMVectorNegate(direction);

	DirectX::XMVECTOR eyePosition = DirectX::XMVectorScale(negativeDirection, 20.0f);
	DirectX::XMMATRIX matrixVector = DirectX::XMMatrixLookToLH(
		eyePosition,
		direction,
		DirectX::XMVectorSet(0.0f, 0.4794255f, 0.0f, 0.8775826f)
	);

	DirectX::XMStoreFloat4x4(&a_lightViewMatrix, matrixVector);
	return a_lightViewMatrix;
}

ID3DBlob* Helper::LoadVertexShaderInternal(const std::wstring& a_fileName, Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader)
{
	ID3DBlob* vertexShaderBlob;
	D3DReadFileToBlob(FixPath(a_fileName).c_str(), &vertexShaderBlob);

	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(), // Pointer to start of binary data
		vertexShaderBlob->GetBufferSize(),// How big is that data?
		0,// No classes in this shader
		a_pVertexShader.GetAddressOf());// ID3D11VertexShader**

	return vertexShaderBlob;
}

void Helper::LoadPixelShaderInternal(const std::wstring& a_fileName, Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader)
{
	ID3DBlob* pixelShaderBlob;
	D3DReadFileToBlob(FixPath(a_fileName).c_str(), &pixelShaderBlob);

	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(), // Pointer to start of binary data
		pixelShaderBlob->GetBufferSize(),// How big is that data?
		0,// No classes in this shader
		a_pPixelShader.GetAddressOf());// ID3D11VertexShader**
}
