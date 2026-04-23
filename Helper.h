#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include "Light.h"

struct Helper
{
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LoadTexture(const wchar_t* a_fileName);

	static DirectX::XMFLOAT4X4 UpdateLightViewMatrix(DirectX::XMFLOAT3 a_lightDirection, DirectX::XMFLOAT4X4 a_lightViewMatrix);
};

