#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <memory>
#include <vector>
#include <array>
#include "Mesh.h"
#include "BufferStructs.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Sky.h"

class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Update(float deltaTime, float totalTime);
	void BuildUI();
	void RefreshGUI(float deltaTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:
	void CreateGeometry();
	void CreateEntities(
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader);

	void CreateLights();
	void CreatePostProcess();

	void CreatePostProcessSRV_RTV();


	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pVSInputLayout;

	// holding data
	int m_number;
	float m_backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	bool m_showDemoWindow = false;
	std::string m_title = "My first window!";
	char m_test[100];

	float m_menuWidth = 400;
	float m_menuHeight = 600;
	bool m_hideHeader = false;

	//Light
	std::array<Light, 5> m_lights;

	//shadow
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pShadowVS;

	//Meshes
	std::shared_ptr<Mesh> m_pCube;
	std::shared_ptr<Mesh> m_pCylinder;
	std::shared_ptr<Mesh> m_pHelix;
	std::shared_ptr<Mesh> m_pPlane;

	//GameEntities
	std::vector<GameEntity> m_entitiesList;

	//Materials
	std::vector<Material> m_materialsList;

	//Cameras
	std::vector<std::shared_ptr<Camera>> m_camerasList;
	std::shared_ptr<Camera> m_pActiveCamera;

	//Updates lights in entity buffers
	void UpdateEntityLights();

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;
	Sky m_sky;

	//shadow
	unsigned int m_shadowMapResolution = 1024;
	unsigned int m_lightProjectionSize = 15;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pShadowDSV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShadowSRV;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pShadowRasterizer;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pShadowSampler;

	DirectX::XMFLOAT4X4 m_lightViewMatrix;
	DirectX::XMFLOAT4X4 m_lightProjectionMatrix;

	//post process
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pPostProcessSampler;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pPostProcessVS;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pPostProcessVSInputLayout;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPostProcessHorizontalGaussPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPostProcessVerticalGaussPS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPostProcessChromaticAbberationPS;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pPostProcessGaussH_RTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pPostProcessGaussH_SRV;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pPostProcessGaussV_RTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pPostProcessGaussV_SRV;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pPostProcessChromaticAbberation_RTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pPostProcessChromaticAbberation_SRV;
	int m_blurAmount = 10;

	DirectX::XMFLOAT3 m_chromaticAbberationOffset = {0.009f, 0.006f, -0.006f};
	//Microsoft::WRL::ComPtr<ID3D11Texture2D*> m_pPostProcessTexture;
};
