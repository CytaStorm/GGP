#include "Camera.h"
#include <algorithm>
#include "Input.h"

Camera::Camera(
	float a_aspectRatio, 
	DirectX::XMFLOAT3 a_initialPosition, DirectX::XMFLOAT4 a_startingOrientation, 
	float a_fovRadians, float a_nearPlane, float a_farPlane, 
	float a_moveSpeed, float a_mouseLookSpeed, Projection a_projectionType)
{
	m_fovRadians = a_fovRadians;
	m_nearPlane = a_nearPlane;
	m_farPlane = a_farPlane;
	m_moveSpeed = a_moveSpeed;
	m_mouseLookSpeed = a_mouseLookSpeed;
	m_projection = a_projectionType;

	m_transform = Transform(a_initialPosition, a_startingOrientation);
	Camera::UpdateProjectionMatrix(a_aspectRatio);
	Camera::UpdateViewMatrix();
}

Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return m_viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float a_aspectRatio)
{
	DirectX::XMMATRIX projectionMatrix;
	switch (m_projection) {
	case Projection::PERSPECTIVE:
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_fovRadians, a_aspectRatio, m_nearPlane, m_farPlane);
		break;
	case Projection::ORTHOGRAPHIC:
		projectionMatrix = DirectX::XMMatrixOrthographicLH(2, 1, m_nearPlane, m_farPlane);
		break;
	default:
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_fovRadians, a_aspectRatio, m_nearPlane, m_farPlane);
		break;
	}
	DirectX::XMStoreFloat4x4(&m_projectionMatrix, projectionMatrix);
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_transform.GetPosition());
	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&m_transform.GetForward());
	DirectX::XMVECTOR worldUpVector = { 0.0f, 1.0f, 0.0f };

	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookToLH(position, forward, worldUpVector);
	DirectX::XMStoreFloat4x4(&m_viewMatrix, viewMatrix);
}

void Camera::Update(float a_deltaTime)
{
	//rotation
	if (Input::MouseLeftDown()) {
		int cursorMovementX = Input::GetMouseXDelta();
		int cursorMovementY = Input::GetMouseYDelta();

		float scaledCursorMovementX = cursorMovementX * m_mouseLookSpeed;
		float scaledCursorMovementY = cursorMovementY * m_mouseLookSpeed;

		//for looking behind
		//if (m_transform.GetForward().z < 0) {
		//	scaledCursorMovementY *= -1;
		//}

		m_lookOffsetPitch += scaledCursorMovementY;
		m_lookOffsetYaw += scaledCursorMovementX;

		if (m_lookOffsetPitch > 1 || m_lookOffsetPitch < -1) {
			m_lookOffsetPitch = std::clamp(m_lookOffsetPitch, -1.0f, 1.0f);
			scaledCursorMovementY = 0;
		}

		// x axis
		DirectX::XMVECTOR rightVector = DirectX::XMLoadFloat3(&m_transform.GetRight());
		DirectX::XMVECTOR pitchOffset = 
			DirectX::XMQuaternionRotationAxis(
				rightVector, scaledCursorMovementY
			);


		// y axis
		DirectX::XMVECTOR upVector = DirectX::XMLoadFloat3(&m_transform.GetUp());
		DirectX::XMVECTOR yawOffset = 
			DirectX::XMQuaternionRotationAxis(
				upVector, scaledCursorMovementX
			);

		//change in rotation
		DirectX::XMVECTOR rotationOffsetVector =
			DirectX::XMQuaternionMultiply(yawOffset, pitchOffset);

		DirectX::XMFLOAT4 rotationOffset;
		DirectX::XMStoreFloat4(&rotationOffset, rotationOffsetVector);

		m_transform.Rotate(rotationOffset);



		//change in rotation
		//DirectX::XMVECTOR rotationOffsetVector = 
		//	DirectX::XMQuaternionRotationRollPitchYaw(
		//		scaledCursorMovementY, scaledCursorMovementX, 0.0f);

		//DirectX::XMFLOAT4 rotationOffset;
		//DirectX::XMStoreFloat4(&rotationOffset, rotationOffsetVector);
		//m_transform.Rotate(rotationOffset);
	}
	//process user input
	//movement
	if (Input::KeyDown('W')) {
		m_transform.MoveRelative(0.0f, 0.0f, m_moveSpeed * a_deltaTime);
	}
	if (Input::KeyDown('S')) {
		m_transform.MoveRelative(0.0f, 0.0f, -m_moveSpeed * a_deltaTime);
	}
	if (Input::KeyDown('D')) {
		m_transform.MoveRelative(m_moveSpeed * a_deltaTime, 0.0f, 0.0f);
	}
	if (Input::KeyDown('A')) {
		m_transform.MoveRelative(-m_moveSpeed * a_deltaTime, 0.0f, 0.0f);
	}
	if (Input::KeyDown(VK_SPACE)) {
		m_transform.MoveRelative(0.0f, m_moveSpeed * a_deltaTime, 0.0f);
	}
	if (Input::KeyDown('X')) {
		m_transform.MoveRelative(0.0f, -m_moveSpeed * a_deltaTime, 0.0f);
	}

	Camera::UpdateViewMatrix();
}

Transform Camera::GetTransform() {
	return m_transform;
}
