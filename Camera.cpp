#include "Camera.h"
#include <algorithm>

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

	//rotation
	if (Input::MouseLeftDown()) {
		int cursorMovementX = Input::GetMouseXDelta();
		int cursorMovementY = Input::GetMouseYDelta();

		float scaledCursorMovementX = cursorMovementX * m_mouseLookSpeed;
		float scaledCursorMovementY = cursorMovementY * m_mouseLookSpeed;


		//m_transform.Rotate(scaledCursorMovementY, scaledCursorMovementX, 0.0f);
		//calculate potential change in rotation, copied mostly from transform.cpp
		if (scaledCursorMovementY == 0.0f && scaledCursorMovementX == 0.0f) {
			return;
		}

		DirectX::XMVECTOR rotationOffsetVector = 
			DirectX::XMQuaternionRotationRollPitchYaw(
				scaledCursorMovementY, scaledCursorMovementX, 0.0f);

		DirectX::XMFLOAT4 originalRotation = m_transform.GetRotation();
		DirectX::XMVECTOR originalRotationVector = DirectX::XMLoadFloat4(&originalRotation);

		DirectX::XMVECTOR potentialRotationVector = DirectX::XMQuaternionMultiply(originalRotationVector, rotationOffsetVector);

		DirectX::XMFLOAT3 potentialForward;
		DirectX::XMVECTOR potentialForwardVector = DirectX::XMLoadFloat3(&m_transform.m_forward);

		DirectX::XMFLOAT3 potentialRight;
		DirectX::XMVECTOR potentialRightVector = DirectX::XMLoadFloat3(&m_transform.m_right);

		potentialForwardVector = DirectX::XMVector3Rotate(potentialForwardVector, rotationOffsetVector);
		potentialRightVector = DirectX::XMVector3Rotate(potentialRightVector, rotationOffsetVector);

		DirectX::XMStoreFloat3(&potentialForward, potentialForwardVector);
		DirectX::XMStoreFloat3(&potentialRight, potentialRightVector);

		DirectX::XMFLOAT4 finalRotation;
		DirectX::XMStoreFloat4(&finalRotation, potentialRotationVector);

		//clamp forward vector (too high)
		if (potentialForward.y > 0.8 || potentialForward.y < -0.8) {
			potentialForward.y = std::clamp(potentialForward.y, -0.8f, 0.8f);
			finalRotation = RotationFromForwardRight(potentialForward, potentialRight);
		}

		m_transform.SetRotation(finalRotation);
	}
	Camera::UpdateViewMatrix();
}

Transform Camera::GetTransform() {
	return m_transform;
}

DirectX::XMFLOAT4 Camera::RotationFromForwardRight(DirectX::XMFLOAT3 a_newForward, DirectX::XMFLOAT3 a_newRight)
{
	DirectX::XMFLOAT3 position = m_transform.GetPosition();
	DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);

	//gets clamped forwards
	DirectX::XMVECTOR forwardVector = DirectX::XMLoadFloat3(&a_newForward);

	//get right (unaffected)
	DirectX::XMVECTOR rightVector = DirectX::XMLoadFloat3(&a_newRight);

	//calculate new up
	DirectX::XMVECTOR upVector = DirectX::XMVector3Cross(forwardVector, rightVector);
	DirectX::XMFLOAT4 up;
	DirectX::XMStoreFloat4(&up, upVector);

	//use new up + forwards to calculate new rotation
	DirectX::XMMATRIX clampedView =
		DirectX::XMMatrixLookToLH(
			positionVector, forwardVector, upVector);

	//load new rotation, recalculate new directions
	DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationMatrix(clampedView);
	DirectX::XMFLOAT4 rotation;
	//Conjugate of rotation b/c view matrix is inverse of camera's transform
	DirectX::XMStoreFloat4(&rotation, DirectX::XMQuaternionConjugate(rotationQuaternion));
	return rotation;
}

