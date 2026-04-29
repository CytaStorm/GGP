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

void Helper::LoadVertexShader(
	Microsoft::WRL::ComPtr<ID3D11InputLayout>& a_pInputLayout,
	Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader,
	const std::wstring a_fileName)
{
	ID3DBlob* vertexShaderBlob = LoadVertexShaderInternal(a_fileName, a_pVertexShader);

	//input layout
	D3D11_INPUT_ELEMENT_DESC inputElements[4] = {};

	// Set up the first element - a position, which is 3 float values
	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
	inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
	inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

	//set up uv coords
	inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElements[1].SemanticName = "TEXCOORD";
	inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	//set up normal
	inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElements[2].SemanticName = "NORMAL";
	inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	//set up tangent
	inputElements[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElements[3].SemanticName = "TANGENT";
	inputElements[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	// Create the input layout, verifying our description against actual shader code
	Graphics::Device->CreateInputLayout(
		inputElements,							// An array of descriptions
		4,										// How many elements in that array?
		vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
		vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
		a_pInputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
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

void Helper::LoadPixelShader(const std::wstring& a_fileName, Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader)
{
	ID3DBlob* pixelShaderBlob;
	D3DReadFileToBlob(FixPath(a_fileName).c_str(), &pixelShaderBlob);

	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(), // Pointer to start of binary data
		pixelShaderBlob->GetBufferSize(),// How big is that data?
		0,// No classes in this shader
		a_pPixelShader.GetAddressOf());// ID3D11VertexShader**
}
