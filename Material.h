#pragma once
#include "BufferStructs.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <unordered_map>

class Material
{
private:
	DirectX::XMFLOAT4 m_colorTint;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
	DirectX::XMFLOAT2 m_UVscale;
	DirectX::XMFLOAT2 m_UVoffset;

	//srvs
	std::unordered_map<unsigned int, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textureSRV;
	std::unordered_map<unsigned int, Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_samplers;


public:
	Material(
		DirectX::XMFLOAT4 a_colorTint,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pVertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> a_pPixelShader);

	Material(
		DirectX::XMFLOAT4 a_colorTint,
		DirectX::XMFLOAT2 a_UVscale,
		DirectX::XMFLOAT2 a_UVoffset,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pVertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> a_pPixelShader);

	~Material();

	//Getters
	DirectX::XMFLOAT4 GetColorTint();
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();
	DirectX::XMFLOAT2 GetUVscale();
	DirectX::XMFLOAT2 GetUVoffset();

	//Setters
	void SetColor(DirectX::XMFLOAT4 a_color);
	void SetVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pVertexShader);
	void SetPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> a_pPixelShader);
	void SetUVScale(int a_xScale, int a_yScale);
	void SetUVOffset(int a_xOffset, int a_yOffset);


	void AddTextureSRV(unsigned int a_index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> a_textureSRV);
	void AddSampler(unsigned int a_index, Microsoft::WRL::ComPtr<ID3D11SamplerState> a_sampler);
	void BindTexturesAndSamplers();
};

