#include "Material.h"
#include <d3dcompiler.h>
#include "PathHelpers.h"
#include "Graphics.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>

Material::Material(
	DirectX::XMFLOAT4 a_color, 
	Microsoft::WRL::ComPtr<ID3D11VertexShader> a_pVertexShader,
	Microsoft::WRL::ComPtr<ID3D11PixelShader> a_pPixelShader)
{
	m_colorTint = a_color;
	m_pVertexShader = a_pVertexShader;
	m_pPixelShader = a_pPixelShader;
}

Material::~Material()
{
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
