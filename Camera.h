#pragma once
#include "Input.h"
#include "Transform.h"
#include <numbers>
#include "Helper.h"
#include "Projection.h"

class Camera
{
private:
	Transform m_transform;
	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;

	/// <summary>
	/// Field of view, in radians
	/// </summary>
	float m_fovRadians = std::numbers::pi_v<float>;
	float m_nearPlane;
	float m_farPlane;
	float m_moveSpeed;
	float m_mouseLookSpeed;

	Projection m_projection = Projection::PERSPECTIVE;

public:
	Camera(
		float a_aspectRatio, 
		DirectX::XMFLOAT3 a_initialPosition, DirectX::XMFLOAT4 a_startingOrientation, 
		float a_fov, float a_nearPlane, float a_farPlane,
		float a_moveSpeed, float a_mouseLookSpeed, Projection a_projectionType);
	~Camera();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	void UpdateProjectionMatrix(float a_aspectRatio);

	void UpdateViewMatrix();

	void Update(float a_deltaTime);

	Transform GetTransform();

	DirectX::XMFLOAT4 RotationFromForwardRight(DirectX::XMFLOAT3 a_newForward, DirectX::XMFLOAT3 a_newRight);
};

