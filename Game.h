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


	// Initialization helper methods - feel free to customize, combine, remove, etc.
	static void LoadVertexShader(
		Microsoft::WRL::ComPtr<ID3D11InputLayout>& a_pInputLayout,
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& a_pVertexShaderConstantBuffer,
		const std::wstring a_fileName);

	//templated function in case other sized constant buffer structs are to be used
	template <typename PSConstantBufferStruct>
	static void LoadPixelShader(
		PSConstantBufferStruct value,
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& a_pPixelShaderConstantBuffer,
		const std::wstring a_fileName);

private:
	void CreateGeometry();
	void CreateEntities(
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader);

	void CreateLights();


	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pVSInputLayout;

	// holding data
	int m_number;
	float m_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	bool m_showDemoWindow = false;
	std::string m_title = "My first window!";
	char m_test[100];

	float m_menuWidth = 400;
	float m_menuHeight = 600;
	bool m_hideHeader = false;

	//Light
	std::array<Light, 5> m_lights;

	//Meshes
	std::shared_ptr<Mesh> m_pCube;
	std::shared_ptr<Mesh> m_pCylinder;
	std::shared_ptr<Mesh> m_pHelix;

	//GameEntities
	std::vector<GameEntity> m_entitiesList;

	//Materials
	std::vector<Material> m_materialsList;

	//Constant buffer for vertex shader
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVSConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pPSConstantBuffer;

	//Cameras
	std::vector<std::shared_ptr<Camera>> m_camerasList;
	std::shared_ptr<Camera> m_pActiveCamera;

	//Updates lights in entity buffers
	void UpdateLights();

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;
	Sky m_sky;
};
