#include "Camera.h"
#include <algorithm>
#include "Input.h"
#include "Helper.h"

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
	DirectX::XMFLOAT3 transformPosition = m_transform.GetPosition();
	DirectX::XMFLOAT3 transformForward = m_transform.GetForward();

	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&transformPosition);
	DirectX::XMVECTOR rotation = DirectX::XMLoadFloat3(&transformForward);
	DirectX::XMFLOAT3 worldUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR worldUpVector = DirectX::XMLoadFloat3(&worldUp);

	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookToLH(position, rotation, worldUpVector);
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

		m_lookOffsetPitch += scaledCursorMovementY;
		m_lookOffsetYaw += scaledCursorMovementX;

		if (m_lookOffsetPitch > 1 || m_lookOffsetPitch < -1) {
			m_lookOffsetPitch = std::clamp(m_lookOffsetPitch, -1.0f, 1.0f);
			scaledCursorMovementY = 0;
		}

		//change in rotation
		DirectX::XMVECTOR rotationOffsetVector = 
			DirectX::XMQuaternionRotationRollPitchYaw(
				scaledCursorMovementY, scaledCursorMovementX, 0.0f);

		DirectX::XMFLOAT4 originalRotation = m_transform.GetRotation();
		DirectX::XMVECTOR originalRotationVector = DirectX::XMLoadFloat4(&originalRotation);

		DirectX::XMVECTOR potentialRotationVector = 
			DirectX::XMQuaternionMultiply(originalRotationVector, rotationOffsetVector);

		DirectX::XMFLOAT3 potentialForward;
		DirectX::XMFLOAT3 transformForward = m_transform.GetForward();
		DirectX::XMVECTOR potentialForwardVector = DirectX::XMLoadFloat3(&transformForward);

		DirectX::XMFLOAT3 potentialRight;
		DirectX::XMFLOAT3 transformRight = m_transform.GetRight();
		DirectX::XMVECTOR potentialRightVector = DirectX::XMLoadFloat3(&transformRight);

		potentialForwardVector = DirectX::XMVector3Rotate(potentialForwardVector, rotationOffsetVector);
		potentialRightVector = DirectX::XMVector3Rotate(potentialRightVector, rotationOffsetVector);

		DirectX::XMStoreFloat3(&potentialForward, potentialForwardVector);
		DirectX::XMStoreFloat3(&potentialRight, potentialRightVector);

		DirectX::XMFLOAT4 finalRotation;
		DirectX::XMStoreFloat4(&finalRotation, potentialRotationVector);

		m_transform.SetRotation(finalRotation);
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
