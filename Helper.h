#pragma once
#include <wrl/client.h>
#include <d3d11.h>

struct Helper
{
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LoadTexture(const wchar_t* a_fileName);
};

