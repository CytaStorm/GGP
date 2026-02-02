#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"

#include <DirectXMath.h>

//ImGui includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "Mesh.h"
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
	LoadShaders();
	CreateGeometry();

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
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
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
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	std::vector<std::shared_ptr<Mesh>> Meshes;
	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> pentagon;
	std::shared_ptr<Mesh> star;

	Meshes.push_back(triangle);
	Meshes.push_back(pentagon);
	Meshes.push_back(star);

	//Triangle
	Vertex trianglePoints[] = {
		{ XMFLOAT3(+0.25f, +0.25f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.25f, +0.0f), blue },
		{ XMFLOAT3(+0.0f, -0.25f, +0.0f), green },
	};
	int triangleIndices[] = { 0, 1, 2 };
	Game::triangle = std::make_shared<Mesh>(trianglePoints, triangleIndices, 3, 3);

	//Pentagon
	Vertex pentagonPoints[] = {
		{ XMFLOAT3(-0.5f, -0.2f, +0.0f), red },
		{ XMFLOAT3(-0.3f, -0.2f, +0.0f), blue },
		{ XMFLOAT3(-0.4f, -0.4f, +0.0f), green },

		{ XMFLOAT3(-0.3f, -0.2f, +0.0f), blue },
		{ XMFLOAT3(-0.2f, -0.5f, +0.0f), red },
		{ XMFLOAT3(-0.4f, -0.4f, +0.0f), green },

		{ XMFLOAT3(-0.4f, -0.4f, +0.0f), green },
		{ XMFLOAT3(-0.2f, -0.5f, +0.0f), red },
		{ XMFLOAT3(-0.4f, -0.7f, +0.0f), blue },

		{ XMFLOAT3(-0.4f, -0.4f, +0.0f), green },
		{ XMFLOAT3(-0.4f, -0.7f, +0.0f), blue },
		{ XMFLOAT3(-0.8f, -0.4f, +0.0f), red },

		{ XMFLOAT3(-0.5f, -0.2f, +0.0f), red },
		{ XMFLOAT3(-0.4f, -0.4f, +0.0f), green },
		{ XMFLOAT3(-0.8f, -0.4f, +0.0f), blue }
	};

	int pentagonIndices[15];

	for (int i = 0; i < 15; i++) {
		pentagonIndices[i] = i;
	}

	Game::pentagon = std::make_shared<Mesh>(pentagonPoints, pentagonIndices, 15, 15);

	//Vertex starPoints[] = {
	//	{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
	//	{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
	//	{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	//};

	////Star
	//int starIndices[] = { 0, 1, 2 };
	//star = std::make_shared<Mesh>(starPoints, starIndices, 30, 30);
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	
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
	ImGui::InputTextMultiline("##source", m_test, IM_ARRAYSIZE(m_test), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));

	if (ImGui::Button("Change title!") && !m_test[0] == '\0') {
		m_title = m_test;
	}

	//Change window size
	ImGui::SliderFloat("How wide would you like the window?", &m_menuWidth, 300.0f, 1000.0f, "%f");
	ImGui::SliderFloat("How high would you like the window?", &m_menuHeight, 400.0f, 720.0f, "%f");

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
		Game::triangle->Draw();
		Game::pentagon->Draw();
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



