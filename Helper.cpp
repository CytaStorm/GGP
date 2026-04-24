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

DirectX::XMFLOAT4X4 Helper::CalculateNewLightViewMatrix(DirectX::XMFLOAT3 a_lightDirection, DirectX::XMFLOAT4X4 a_lightViewMatrix)
{
	//edge case
	DirectX::XMVECTOR worldup = DirectX::XMVectorSet(0, 1, 0, 0);
	if (a_lightDirection.x == 0 && a_lightDirection.y == -1 && a_lightDirection.z == 0) {
		worldup = DirectX::XMVectorSet(0, 0, 0.4794255, 0.8775826);
		//worldup = DirectX::XMVectorSet(0, 0, 1, 0);
	}

	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&a_lightDirection);
	DirectX::XMVECTOR negativeDirection = DirectX::XMVectorNegate(direction);

	DirectX::XMVECTOR eyePosition = DirectX::XMVectorScale(negativeDirection, 20.0f);
	DirectX::XMMATRIX matrixVector = DirectX::XMMatrixLookToLH(
		eyePosition,
		direction,
		DirectX::XMVectorSet(0, 0.4794255, 0, 0.8775826)
	);

	DirectX::XMStoreFloat4x4(&a_lightViewMatrix, matrixVector);
	return a_lightViewMatrix;
}
