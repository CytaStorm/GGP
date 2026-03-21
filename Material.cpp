#include "Material.h"
#include <d3dcompiler.h>
#include "PathHelpers.h"
#include "Graphics.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>

Material::Material(DirectX::XMFLOAT4 a_colorTint, Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pVertexShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> a_pPixelShader) : 
	Material(
		a_colorTint,
		DirectX::XMFLOAT2(1, 1),
		DirectX::XMFLOAT2(0, 0),
		a_pVertexShader,
		a_pPixelShader)
{
}

Material::Material(
	DirectX::XMFLOAT4 a_color, 
	DirectX::XMFLOAT2 a_UVscale, 
	DirectX::XMFLOAT2 a_UVoffset, 
	Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pVertexShader,
	Microsoft::WRL::ComPtr<ID3D11PixelShader> a_pPixelShader)
{
	m_colorTint = a_color;
	m_pVertexShader = a_pVertexShader;
	m_pPixelShader = a_pPixelShader;
	m_UVscale = a_UVscale;
	m_UVoffset = a_UVoffset;
}

Material::~Material()
{
}

DirectX::XMFLOAT2 Material::GetUVscale() {
	return m_UVscale;
}

DirectX::XMFLOAT2 Material::GetUVoffset() {
	return m_UVoffset;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return m_colorTint;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVertexShader()
{
	return m_pVertexShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPixelShader()
{
	return m_pPixelShader;
}

void Material::SetColor(DirectX::XMFLOAT4 a_color)
{
	m_colorTint = a_color;
}

void Material::SetVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pVertexShader)
{
	m_pVertexShader = a_pVertexShader;
}

void Material::SetPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> a_pPixelShader)
{
	m_pPixelShader = a_pPixelShader;
}

void Material::SetUVScale(int a_xScale, int a_yScale) {
	m_UVscale.x = a_xScale;
	m_UVscale.y = a_yScale;
}

void Material::SetUVOffset(int a_xOffset, int a_yOffset) {
	m_UVoffset.x = a_xOffset;
	m_UVoffset.y = a_yOffset;
}

void Material::BindTexturesAndSamplers()
{
	for (const auto& [key, value] : m_textureSRV) {
		Graphics::Context->PSSetShaderResources(key, 1, value.GetAddressOf());
	}

	for (const auto& [key, value] : m_samplers) {
		Graphics::Context->PSSetSamplers(key, 1, value.GetAddressOf());
	}
}

void Material::AddTextureSRV(unsigned int a_index, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> a_textureSRV)
{
	m_textureSRV[a_index] = a_textureSRV;
}

void Material::AddSampler(unsigned int a_index, Microsoft::WRL::ComPtr<ID3D11SamplerState> a_sampler)
{
	m_samplers[a_index] = a_sampler;
}
