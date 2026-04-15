#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"
#include "Camera.h"
#include "Projection.h"
#include <DirectXMath.h>
#include "WICTextureLoader.h"
#include "Helper.h"

//ImGui includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <vector>
#include <memory>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	IMGUI_CHECKVERSION
	();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(
		Window::Handle());

	ImGui_ImplDX11_Init(
		Graphics::Device.Get(), Graphics::Context.Get());

	ImGui::StyleColorsDark();

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> uvPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> normalsPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> customPixelShader;

	LoadVertexShader(m_pVSInputLayout, vertexShader, m_pVSConstantBuffer, L"VertexShader.cso");
	LoadPixelShader<PSConstantBuffer>(
		PSConstantBuffer(), pixelShader, m_pPSConstantBuffer, L"PixelShader.cso");
	LoadPixelShader<PSConstantBuffer>(
		PSConstantBuffer(), uvPixelShader, m_pPSConstantBuffer, L"DebugUVsPS.cso");
	LoadPixelShader<PSConstantBuffer>(
		PSConstantBuffer(), normalsPixelShader, m_pPSConstantBuffer, L"DebugNormalsPS.cso");
	LoadPixelShader<PSConstantBuffer>(
		PSConstantBuffer(), customPixelShader, m_pPSConstantBuffer, L"CustomPS.cso");

	//Set constant buffers
	Graphics::Context->VSSetConstantBuffers(0, 1, m_pVSConstantBuffer.GetAddressOf());

	Graphics::Context->PSSetConstantBuffers(0, 1, m_pPSConstantBuffer.GetAddressOf());

	//Sky
	CreateLights();
	CreateGeometry();

	//sampler state created in here
	CreateEntities(vertexShader, pixelShader);

	m_sky = Sky(
		m_pCube,
		L"Assets/Textures/Skies/Planet/front.png",
		L"Assets/Textures/Skies/Planet/back.png",
		L"Assets/Textures/Skies/Planet/left.png",
		L"Assets/Textures/Skies/Planet/right.png",
		L"Assets/Textures/Skies/Planet/up.png",
		L"Assets/Textures/Skies/Planet/down.png",
		L"VertexShader_Sky.cso",
		L"PixelShader_Sky.cso",
		m_pSamplerState		
	);


	std::shared_ptr<Material> uvMaterial = std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		vertexShader,
		uvPixelShader);
	std::shared_ptr<Material> normalsMaterial = std::make_shared<Material>(
		DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		vertexShader,
		normalsPixelShader);
	std::shared_ptr<Material> customMaterial = std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		vertexShader,
		customPixelShader);

	m_entitiesList.push_back(GameEntity(m_pCube, uvMaterial));
	m_entitiesList.push_back(GameEntity(m_pCylinder, uvMaterial));
	m_entitiesList.push_back(GameEntity(m_pHelix, uvMaterial));
	m_entitiesList[3].GetTransform().MoveAbsolute(3.0f, 0.0f, 10.0f);
	m_entitiesList[4].GetTransform().MoveAbsolute(6.0f, 0.0f, 0.0f);
	m_entitiesList[5].GetTransform().MoveAbsolute(9.0f, 0.0f, -10.0f);

	m_entitiesList.push_back(GameEntity(m_pCube, normalsMaterial));
	m_entitiesList.push_back(GameEntity(m_pCylinder, normalsMaterial));
	m_entitiesList.push_back(GameEntity(m_pHelix, normalsMaterial));
	m_entitiesList[6].GetTransform().MoveAbsolute(-3.0f, 0.0f, 10.0f);
	m_entitiesList[7].GetTransform().MoveAbsolute(-6.0f, 0.0f, 0.0f);
	m_entitiesList[8].GetTransform().MoveAbsolute(-12.0f, 0.0f, -10.0f);

	m_entitiesList.push_back(GameEntity(m_pCube, customMaterial));
	m_entitiesList.push_back(GameEntity(m_pCylinder, customMaterial));
	m_entitiesList.push_back(GameEntity(m_pHelix, customMaterial));
	m_entitiesList[9].GetTransform().MoveAbsolute(6.0f, 0.0f, 10.0f);
	m_entitiesList[10].GetTransform().MoveAbsolute(9.0f, 0.0f, 0.0f);
	m_entitiesList[11].GetTransform().MoveAbsolute(12.0f, 0.0f, -10.0f);

	//assign lights
	for (GameEntity& entity : m_entitiesList) {
		entity.m_PSConstantBuffer.m_lights = m_lights;
	}

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(m_pVSInputLayout.Get());
	}

	m_number = 1;
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadVertexShader(
	Microsoft::WRL::ComPtr<ID3D11InputLayout>& a_pInputLayout,
	Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader,
	Microsoft::WRL::ComPtr<ID3D11Buffer>& a_pVertexShaderConstantBuffer,
	std::wstring a_fileName)
{
	ID3DBlob* vertexShaderBlob;
	D3DReadFileToBlob(FixPath(a_fileName).c_str(), &vertexShaderBlob);

	Graphics::Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(), // Pointer to start of binary data
		vertexShaderBlob->GetBufferSize(),// How big is that data?
		0,// No classes in this shader
		a_pVertexShader.GetAddressOf());// ID3D11VertexShader**
	
	//input layout
	D3D11_INPUT_ELEMENT_DESC inputElements[4] = {};

	// Set up the first element - a position, which is 3 float values
	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
	inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
	inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

	//set up uv coords
	inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;		
	inputElements[1].SemanticName = "TEXCOORD";							
	inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	

	//set up normal
	inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;		
	inputElements[2].SemanticName = "NORMAL";							
	inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	

	//set up tangent
	inputElements[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;		
	inputElements[3].SemanticName = "TANGENT";							
	inputElements[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	

	// Create the input layout, verifying our description against actual shader code
	Graphics::Device->CreateInputLayout(
		inputElements,							// An array of descriptions
		4,										// How many elements in that array?
		vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
		vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
		a_pInputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer

	//Create & bind vertex shader constant buffer
	D3D11_BUFFER_DESC VS_ConstantBufferDesc = {};
	VS_ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	VS_ConstantBufferDesc.ByteWidth = (sizeof(VertexShaderConstantBuffer) + 15) / 16 * 16;
	VS_ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VS_ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	Graphics::Device->CreateBuffer(&VS_ConstantBufferDesc, 0, a_pVertexShaderConstantBuffer.GetAddressOf());
}

template <typename PSConstantBufferStruct>
void Game::LoadPixelShader(
	PSConstantBufferStruct a_pixelBufferStruct,
	Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader,
	Microsoft::WRL::ComPtr<ID3D11Buffer>& a_pPixelShaderConstantBuffer,
	std::wstring a_fileName)
{
	ID3DBlob* pixelShaderBlob;
	D3DReadFileToBlob(FixPath(a_fileName).c_str(), &pixelShaderBlob);

	Graphics::Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(), // Pointer to start of binary data
		pixelShaderBlob->GetBufferSize(),// How big is that data?
		0,// No classes in this shader
		a_pPixelShader.GetAddressOf());// ID3D11VertexShader**

	a_pPixelShaderConstantBuffer.Reset();
	//Create & bind pixel shader constant buffer
	D3D11_BUFFER_DESC PS_ConstantBufferDesc = {};
	PS_ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	PS_ConstantBufferDesc.ByteWidth = (sizeof(PSConstantBufferStruct) + 15) / 16 * 16;
	PS_ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	PS_ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	
	Graphics::Device->CreateBuffer(&PS_ConstantBufferDesc, 0, a_pPixelShaderConstantBuffer.GetAddressOf());
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	m_pCube = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cube.obj").c_str());
	m_pCylinder = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cylinder.obj").c_str());
	m_pHelix = std::make_shared<Mesh>(FixPath("../../Assets/Meshes/helix.obj").c_str());
}

void Game::CreateEntities(
	Microsoft::WRL::ComPtr<ID3D11VertexShader>& a_pVertexShader,
	Microsoft::WRL::ComPtr<ID3D11PixelShader>& a_pPixelShader)
{
	//Create Cameras
	std::shared_ptr cameraPerspective = std::make_shared<Camera>(
		Window::AspectRatio(),
		DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		0.5f * std::numbers::pi_v<float>,
		0.01f,
		999.0f,
		1.0f,
		0.01f,
		Projection::PERSPECTIVE
	);

	std::shared_ptr cameraOrthographic = std::make_shared<Camera>(
		Window::AspectRatio(),
		DirectX::XMFLOAT3(-0.5f, 0.0f, -1.0f),
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		0.4f * std::numbers::pi_v<float>,
		0.01f,
		999.0f,
		1.0f,
		0.01f,
		Projection::ORTHOGRAPHIC
	);

	m_camerasList.push_back(cameraPerspective);
	m_camerasList.push_back(cameraOrthographic);

	m_pActiveCamera = cameraOrthographic;


	auto flatNormalSRV = Helper::LoadTexture(L"Assets/Textures/flat_normals.png");

	//bronze
	auto bronzeAlbedo = Helper::LoadTexture(L"Assets/Textures/bronze_albedo.png");
	auto bronzeRoughness = Helper::LoadTexture(L"Assets/Textures/bronze_roughness.png");
	auto bronzeMetallic = Helper::LoadTexture(L"Assets/Textures/bronze_metal.png");
	auto bronzeNormal = Helper::LoadTexture(L"Assets/Textures/bronze_normals.png");

	//scratched
	auto scratchedAlbedo = Helper::LoadTexture(L"Assets/Textures/scratched_albedo.png");
	auto scratchedRoughness = Helper::LoadTexture(L"Assets/Textures/scratched_roughness.png");
	auto scratchedMetallic = Helper::LoadTexture(L"Assets/Textures/scratched_metal.png");
	auto scratchedNormal = Helper::LoadTexture(L"Assets/Textures/scratched_normals.png");

	//floor
	auto floorAlbedo = Helper::LoadTexture(L"Assets/Textures/floor_albedo.png");
	auto floorRoughness = Helper::LoadTexture(L"Assets/Textures/floor_roughness.png");
	auto floorMetallic = Helper::LoadTexture(L"Assets/Textures/floor_metal.png");
	auto floorNormal = Helper::LoadTexture(L"Assets/Textures/floor_normals.png");

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;

	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Graphics::Device->CreateSamplerState(&samplerDesc, m_pSamplerState.GetAddressOf());
	
	//create game entities and their materials
	std::shared_ptr<Material> red = std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT2(5.0f, 5.0f),
		DirectX::XMFLOAT2(0.0f, 0.0f),
		a_pVertexShader,
		a_pPixelShader);
	std::shared_ptr<Material> white = std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		a_pVertexShader,
		a_pPixelShader);
	std::shared_ptr<Material> green = std::make_shared<Material>(
		DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		a_pVertexShader,
		a_pPixelShader);

	red->AddTextureSRV(0, bronzeAlbedo);
	red->AddTextureSRV(1, bronzeNormal);
	red->AddTextureSRV(2, bronzeRoughness);
	red->AddTextureSRV(3, bronzeMetallic);
	red->AddSampler(0, m_pSamplerState);

	white->AddTextureSRV(0, floorAlbedo);
	white->AddTextureSRV(1, floorNormal);
	white->AddTextureSRV(2, floorRoughness);
	white->AddTextureSRV(3, floorMetallic);
	white->AddSampler(0, m_pSamplerState);

	green->AddTextureSRV(0, scratchedAlbedo);
	green->AddTextureSRV(1, scratchedNormal);
	green->AddTextureSRV(2, scratchedRoughness);
	green->AddTextureSRV(3, scratchedMetallic);
	green->AddSampler(0, m_pSamplerState);

	m_entitiesList.push_back(GameEntity(m_pCube, red));
	m_entitiesList.push_back(GameEntity(m_pCylinder, white));
	m_entitiesList.push_back(GameEntity(m_pHelix, green));

	m_entitiesList[0].GetTransform().MoveAbsolute(0.0f, 0.0f, 10.0f);
	m_entitiesList[1].GetTransform().MoveAbsolute(3.0f, 0.0f, 0.0f);
	m_entitiesList[2].GetTransform().MoveAbsolute(6.0f, 0.0f, -10.0f);
}

void Game::CreateLights()
{
	//directional
	m_lights[0].m_Type = LIGHT_TYPE_DIRECTIONAL;
	m_lights[0].m_Direction = DirectX::XMFLOAT3{-1.0f, 0.0f, 0.0f};
	m_lights[0].m_Color = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};
	m_lights[0].m_Intensity = 1.0f;

	m_lights[1].m_Type = LIGHT_TYPE_POINT;
	m_lights[1].m_Position = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
	m_lights[1].m_Color = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};
	m_lights[1].m_Intensity = 1.0f;
	m_lights[1].m_Range = 10.0f;

	m_lights[2].m_Type = LIGHT_TYPE_DIRECTIONAL;
	m_lights[2].m_Direction = DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f};
	m_lights[2].m_Color = DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f};
	m_lights[2].m_Intensity = 1.0f;

	m_lights[3].m_Type = LIGHT_TYPE_DIRECTIONAL;
	m_lights[3].m_Direction = DirectX::XMFLOAT3{0.0f, -1.0f, 0.0f};
	m_lights[3].m_Color = DirectX::XMFLOAT3{1.0f, 0.0f, 1.0f};
	m_lights[3].m_Intensity = 1.0f;

	m_lights[4].m_Type = LIGHT_TYPE_SPOT;
	m_lights[4].m_Direction = DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f};
	m_lights[4].m_Color = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};
	m_lights[4].m_Position = DirectX::XMFLOAT3{4.0f, 0.0f, -2.0f};
	m_lights[4].m_SpotOuterAngle = 80 * 3.14f / 180;
	m_lights[4].m_SpotInnerAngle = 60 * 3.14f / 180;
	m_lights[4].m_Intensity = 1.0f;
	m_lights[4].m_Range = 100.0f;
}



// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for (std::shared_ptr<Camera> camera : m_camerasList) {
		if (camera == nullptr) return;
		camera->UpdateProjectionMatrix(Window::AspectRatio());
	}

}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	RefreshGUI(deltaTime);
	BuildUI();
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	for (GameEntity& ent : m_entitiesList) {
		ent.m_lifetimeMs += deltaTime;
	}
	//for (GameEntity& object : m_entitiesList) {
	//	object.GetTransform().MoveAbsolute(sin(totalTime * 2) * deltaTime, sin(totalTime * 2) * deltaTime, 0.0f);
	//}
	m_pActiveCamera->Update(deltaTime);
}

//Builds custom GUI
void Game::BuildUI() {
	//building window
	ImGui::SetNextWindowSize({m_menuWidth, m_menuHeight});

	//hide header?
	if (m_hideHeader) {
		ImGui::Begin(m_title.c_str(), NULL, ImGuiWindowFlags_NoTitleBar);
	}
	else {
		ImGui::Begin(m_title.c_str());
	}
		
	if (ImGui::TreeNode("Window stats"))
	{
		ImGui::Text("Framerate: %f", ImGui::GetIO().Framerate);
		ImGui::Text("Window dimensions: %dx%d", Window::Width(), Window::Height());
		ImGui::TreePop();
	}

	ImGui::ColorEdit4("Background Color", m_color);
		
	if (ImGui::Button("Press to toggle demo window!")) {
		m_showDemoWindow = !m_showDemoWindow;
	}

	if (m_showDemoWindow) {
		ImGui::ShowDemoWindow(&m_showDemoWindow);
	}
	
	//Change window title
	ImGui::Text("Change the window title here!");
	ImGui::InputTextMultiline(
		"##source", m_test, IM_ARRAYSIZE(m_test), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));

	if (ImGui::Button("Change title!") && !m_test[0] == '\0') {
		m_title = m_test;
	}

	//Change window size
	ImGui::SliderFloat("How wide would you like the window?", &m_menuWidth, 300.0f, 1000.0f, "%f");
	ImGui::SliderFloat("How high would you like the window?", &m_menuHeight, 400.0f, 720.0f, "%f");

	if (ImGui::TreeNode("Scene Objects")) {
		for (int i = 1; i < 13; i++) {
			ImGui::PushID(i);
			if (ImGui::TreeNode("", "Object %d", i)) {
				GameEntity* currentObject = &m_entitiesList[i - 1];
				DirectX::XMFLOAT3 position = currentObject->GetTransform().GetPosition();
				DirectX::XMFLOAT4 rotation = currentObject->GetTransform().GetRotation();
				DirectX::XMFLOAT3 scale = currentObject->GetTransform().GetScale();

				if (ImGui::DragFloat3("Translation", &position.x, 0.1f, -1.0f, 1.0f)) {
					currentObject->GetTransform().SetPosition(position);
				};
				if (ImGui::DragFloat4("Rotation (Quaternion)", &rotation.x, 0.1f, -1.0f, 1.0f)) {
					currentObject->GetTransform().SetRotation(rotation);
				}
				if (ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.1f, 2.0f)) {
					currentObject->GetTransform().SetScale(scale);
				}

				int vertexCount = (currentObject->GetMesh()).get()->GetVertexCount();
				int triangleCount = vertexCount / 3;
				int indexCount = (currentObject->GetMesh()).get()->GetIndexCount();

				ImGui::Text("Triangles: %d", triangleCount);
				ImGui::Text("Vertices: %d", vertexCount);
				ImGui::Text("Indicies: %d", indexCount);
				
				if (ImGui::TreeNode("Material")) {
					std::shared_ptr<Material> currentMaterial = currentObject->GetMaterial();
					DirectX::XMFLOAT4 tint = currentMaterial->GetColorTint();
					if (ImGui::DragFloat4("Tint", &tint.x, 0.05f, 0.0f, 1.0f)) {
						currentMaterial->SetColor(tint);
					}
					DirectX::XMFLOAT2 UVscale = currentMaterial->GetUVscale();
					if (ImGui::DragFloat2("UV Scale", &UVscale.x, 0.1f, 0.0f, 10.0f)) {
						currentMaterial->SetUVScale(UVscale.x, UVscale.y);
					}
					DirectX::XMFLOAT2 UVoffset = currentMaterial->GetUVoffset();
					if (ImGui::DragFloat2("UV Offset", &UVoffset.x, 0.1f, 0.0f, 10.0f)) {
						currentMaterial->SetUVOffset(UVoffset.x, UVoffset.y);
					}
					for (const auto& [key, value] : currentMaterial->GetSRVMap()) {
						ImGui::Image((void*)value.Get(), ImVec2(100, 100));
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	//Change Shape tint

	ImGui::SeparatorText("Cameras");

	static int e = 1;
	if (ImGui::RadioButton("Camera 1: Perspective", &e, 0)) {
		m_pActiveCamera = m_camerasList[0];
	}
	if (ImGui::RadioButton("Camera 2: Orthographic", &e, 1)) {
		m_pActiveCamera = m_camerasList[1];
	}

	if (ImGui::TreeNode("Active Camera")) {
		//Camera
		DirectX::XMFLOAT3 position = m_pActiveCamera->GetTransform().GetPosition();
		DirectX::XMFLOAT4 rotation = m_pActiveCamera->GetTransform().GetRotation();
		DirectX::XMFLOAT3 scale = m_pActiveCamera->GetTransform().GetScale();

		if (ImGui::DragFloat3("Translation", &position.x, 0.1f, -1.0f, 1.0f)) {
			m_pActiveCamera->GetTransform().SetPosition(position);
		};
		if (ImGui::DragFloat4("Rotation (Quaternion)", &rotation.x, 0.1f, -1.0f, 1.0f)) {
			m_pActiveCamera->GetTransform().SetRotation(rotation);
		}
		if (ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.1f, 2.0f)) {
			m_pActiveCamera->GetTransform().SetScale(scale);
		}
		DirectX::XMFLOAT3 forwards = m_pActiveCamera->GetTransform().GetForward();
		if (ImGui::DragFloat3("Forwards", &forwards.x, 0.1f, 0.1f, 2.0f)) {
		}

		DirectX::XMFLOAT3 right = m_pActiveCamera->GetTransform().GetRight();
		if (ImGui::DragFloat3("Right", &right.x, 0.1f, 0.1f, 2.0f)) {
		}

		DirectX::XMFLOAT3 up = m_pActiveCamera->GetTransform().GetUp();
		if (ImGui::DragFloat3("Up", &up.x, 0.1f, 0.1f, 2.0f)) {
		}

		if (ImGui::DragFloat("Look X", &(m_pActiveCamera->m_lookOffsetPitch), 0.1f, 0.1f, 2.0f)) {
		}
		ImGui::TreePop();
	};


	if (ImGui::TreeNode("Lights")) {
		if (ImGui::DragFloat3("Ambient Lighting Color", &m_ambientColor.x, 0.1f, 0.0f, 1.0f)) {

		}

		for (Light& light : m_lights) {
			ImGui::PushID(&light);
			const char* lightType;
			switch (light.m_Type) {
			case 0:
				lightType = "Directional";
				break;
			case 1:
				lightType = "Point";
				break;
			case 2:
				lightType = "Spot";
				break;
			default:
				lightType = "ERROR";
				break;
			}

			if (ImGui::TreeNode("", "%s Light", lightType)) {
				if (ImGui::DragFloat3("Position", &light.m_Position.x, 0.1f, 0.0f, 10.0f))
				{
					UpdateLights();
				}
				if (ImGui::DragFloat3("Direction", &light.m_Direction.x, 0.1f, -10.0f, 10.0f)) {
					//normalize direction
					DirectX::XMVECTOR normalized = DirectX::XMLoadFloat3(&light.m_Direction);
					normalized = DirectX::XMVector3Normalize(normalized);
					DirectX::XMStoreFloat3(&light.m_Direction, normalized);
					UpdateLights();
				}
				if (ImGui::DragFloat3("Color", &light.m_Color.x, 0.1f, 0.0f, 1.0f))
				{
					UpdateLights();
				}
				if (ImGui::DragFloat("Intensity", &light.m_Intensity, 0.1f, 0.0f, 10.0f))
				{
					UpdateLights();
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	//hide header
	ImGui::Checkbox("Hide header?", &m_hideHeader);
	//ending
	ImGui::End();
	
};

/// <summary>
/// Updates GUI
/// </summary>
void Game::RefreshGUI(float deltaTime) {
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();

	//reset frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
	}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*

	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	m_color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	//{
	//	// Set buffers in the input assembler (IA) stage
	//	//  - Do this ONCE PER OBJECT, since each object may have different geometry
	//	//  - For this demo, this step *could* simply be done once during Init()
	//	//  - However, this needs to be done between EACH DrawIndexed() call
	//	//     when drawing different geometry, so it's here as an example
	//	UINT stride = sizeof(Vertex);
	//	UINT offset = 0;
	//	Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	//	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//	// Tell Direct3D to draw
	//	//  - Begins the rendering pipeline on the GPU
	//	//  - Do this ONCE PER OBJECT you intend to draw
	//	//  - This will use all currently set Direct3D resources (shaders, buffers, etc)
	//	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//	//     vertices in the currently set VERTEX BUFFER
	//	Graphics::Context->DrawIndexed(
	//		3,     // The number of indices to use (we could draw a subset if we wanted)
	//		0,     // Offset to the first index we want to use
	//		0);    // Offset to add to each index when looking up vertices
	//}

	{
		for (GameEntity& entity : m_entitiesList) {
			entity.GetMaterial()->BindTexturesAndSamplers();
			entity.Draw(m_pVSConstantBuffer, m_pPSConstantBuffer, m_pActiveCamera, m_ambientColor);
		}
		m_sky.Draw(m_pActiveCamera);
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		ImGui::Render(); //renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); //draws to screen
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}

}

void Game::UpdateLights() {
	for (GameEntity& entity : m_entitiesList) {
		entity.m_PSConstantBuffer.m_lights = m_lights;
	}
}



