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
	template<typename VSConstantBufferStruct>
	static void LoadVertexShaderWithConstantBuffer(
		Microsoft::WRL::ComPtr<ID3D11InputLayout>& a_pInputLayout,
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& a_pVertexShaderConstantBuffer,
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

		a_pVertexShaderConstantBuffer.Reset();
		//Create & bind vertex shader constant buffer
		D3D11_BUFFER_DESC VS_ConstantBufferDesc = {};
		VS_ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		VS_ConstantBufferDesc.ByteWidth = (sizeof(VSConstantBufferStruct) + 15) / 16 * 16;
		VS_ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		VS_ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		Graphics::Device->CreateBuffer(&VS_ConstantBufferDesc, 0, a_pVertexShaderConstantBuffer.GetAddressOf());
	}

	static ID3DBlob* LoadVertexShaderInternal(
		const std::wstring& a_fileName,
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader);

	//templated function in case other sized constant buffer structs are to be used
	template <typename PSConstantBufferStruct>
	static void LoadPixelShaderWithConstantBuffer(
		PSConstantBufferStruct value,
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& a_pPixelShaderConstantBuffer,
		const std::wstring a_fileName)
	{
		LoadPixelShaderInternal(a_fileName, a_pPixelShader);

		a_pPixelShaderConstantBuffer.Reset();
		//Create & bind pixel shader constant buffer
		D3D11_BUFFER_DESC PS_ConstantBufferDesc = {};
		PS_ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		PS_ConstantBufferDesc.ByteWidth = (sizeof(PSConstantBufferStruct) + 15) / 16 * 16;
		PS_ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		PS_ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		Graphics::Device->CreateBuffer(&PS_ConstantBufferDesc, 0, a_pPixelShaderConstantBuffer.GetAddressOf());
	}

	static void LoadPixelShaderInternal(
		const std::wstring& a_fileName, 
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader);

};

