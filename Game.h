#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <memory>
#include <vector>
#include "Mesh.h"
#include "BufferStructs.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Material.h"

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

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders(
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader, 
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader);
	void CreateGeometry();
	void CreateEntities(
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader, 
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader);

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

	// holding data
	int m_number;
	float m_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	bool m_showDemoWindow = false;
	std::string m_title = "My first window!";
	char m_test[100];

	float m_menuWidth = 400;
	float m_menuHeight = 600;
	bool m_hideHeader = false;

	//Meshes
	std::shared_ptr<Mesh> m_cube;
	std::shared_ptr<Mesh> m_cylinder;
	std::shared_ptr<Mesh> m_helix;

	//GameEntities
	std::vector<GameEntity> m_entitiesList;

	//Materials
	std::vector<Material> m_materialsList;

	//Constant buffer for vertex shader
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSConstantBuffer;

	//Cameras
	std::vector<std::shared_ptr<Camera>> m_camerasList;
	std::shared_ptr<Camera> m_activeCamera;
};

