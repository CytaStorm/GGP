#pragma once
#include <wrl/client.h>
#include <d3d11.h>

struct Helper
{
	void static LoadVertexShader(
		ID3DBlob** a_pVertexShaderBlob, Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader);
	void static LoadPixelShader(
		ID3DBlob** a_pPixelShaderBlob, Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader);
};

