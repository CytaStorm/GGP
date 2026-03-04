#pragma once
#include "BufferStructs.h"
#include <wrl/client.h>
#include <d3d11.h>

class Material
{
private:
	DirectX::XMFLOAT4 m_color;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;

public:
	Material(
		DirectX::XMFLOAT4 a_color,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pVertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> a_pPixelShader);

	~Material();

	//Getters
	DirectX::XMFLOAT4 GetColor();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();

	//Setters
	void SetColor(DirectX::XMFLOAT4 a_color);
	void SetVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pVertexShader);
	void SetPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> a_pPixelShader);
};

