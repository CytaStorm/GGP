#include "Helper.h"
#include "WICTextureLoader.h"
#include "Graphics.h"

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
