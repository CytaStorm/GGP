#include "Transform.h"

Transform::Transform() : Transform(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

Transform::Transform(DirectX::XMFLOAT3 a_startingPosition, DirectX::XMFLOAT4 a_startingOrientation)
{
	m_position = a_startingPosition;
	m_rotation = a_startingOrientation;
	m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	UpdateTransformDirection(m_rotation);

	DirectX::XMStoreFloat4x4(&m_worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::~Transform()
{

}

void Transform::SetPosition(DirectX::XMFLOAT3 a_position)
{	
	if (a_position.x == m_position.x &&
		a_position.y == m_position.y &&
		a_position.z == m_position.z) {
		return;
	}

	//only update if different, update dirty
	m_position = a_position;
	m_matrixDirtied = true;
}

void Transform::SetPosition(float a_x, float a_y, float a_z)
{
	//only update if different, update dirty
	Transform::SetPosition(DirectX::XMFLOAT3(a_x, a_y, a_z));
}

void Transform::SetRotation(DirectX::XMFLOAT4 a_rotation)
{
	if (a_rotation.x == m_rotation.x &&
		a_rotation.y == m_rotation.y &&
		a_rotation.z == m_rotation.z &&
		a_rotation.w == m_rotation.w) {
		return;
	}

	//only update if different, update dirty
	m_rotation = a_rotation;


	//Reset transform, since is setting absolute rotation
	m_right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_forward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	
	UpdateTransformDirection(a_rotation);

	m_matrixDirtied = true;
}

void Transform::SetRotation(float a_x, float a_y, float a_z, float a_w)
{
	Transform::SetRotation(DirectX::XMFLOAT4(a_x, a_y, a_z, a_w));
}

void Transform::SetScale(DirectX::XMFLOAT3 a_scale)
{
	if (a_scale.x == m_scale.x &&
		a_scale.y == m_scale.y &&
		a_scale.z == m_scale.z) {
		return;
	}

	//only update if different, update dirty
	m_scale = a_scale;
	m_matrixDirtied = true;
}

void Transform::SetScale(float a_x, float a_y, float a_z)
{
	Transform::SetScale(DirectX::XMFLOAT3(a_x, a_y, a_z));
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return m_position;
}

DirectX::XMFLOAT4 Transform::GetRotation()
{
	return m_rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return m_scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	return m_worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return m_worldInverseTransposeMatrix;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 a_offset)
{
	if (a_offset.x == 0 && a_offset.y == 0 && a_offset.z == 0) {
		return;
	}

	//Only update when offset not zero

	DirectX::XMVECTOR originalPosition = DirectX::XMLoadFloat3(&m_position);
	DirectX::XMVECTOR newPosition = DirectX::XMVectorAdd(originalPosition, DirectX::XMLoadFloat3(&a_offset));

	DirectX::XMStoreFloat3(&m_position, newPosition);
	m_matrixDirtied = true;
}

void Transform::MoveAbsolute(float a_x, float a_y, float a_z)
{
	//Only update when offset not zero
	Transform::MoveAbsolute(DirectX::XMFLOAT3(a_x, a_y, a_z));
}

void Transform::Rotate(float a_pitch, float a_yaw, float a_roll)
{
	if (a_pitch == 0 && a_yaw == 0 && a_roll == 0) {
		return;
	}

	//Only update when offset not zero
	DirectX::XMVECTOR rotationOffsetVector = DirectX::XMQuaternionRotationRollPitchYaw(a_pitch, a_yaw, a_roll);
	DirectX::XMVECTOR originalRotation = DirectX::XMLoadFloat4(&m_rotation);
	DirectX::XMVECTOR finalRotation = DirectX::XMQuaternionMultiply(originalRotation, rotationOffsetVector);

	DirectX::XMStoreFloat4(&m_rotation, finalRotation);
	UpdateTransformDirection(rotationOffsetVector);
	m_matrixDirtied = true;
}

void Transform::Rotate(DirectX::XMFLOAT4 a_rotation)
{
	if (a_rotation.x == 0 &&
		a_rotation.y == 0 &&
		a_rotation.z == 0 &&
		a_rotation.w == 1) {
		return;
	}

	//Only update when offset not zero
	DirectX::XMVECTOR rotationOffsetVector = DirectX::XMLoadFloat4(&a_rotation);
	DirectX::XMVECTOR originalRotation = DirectX::XMLoadFloat4(&m_rotation);
	rotationOffsetVector = DirectX::XMQuaternionMultiply(originalRotation, rotationOffsetVector);

	DirectX::XMStoreFloat4(&m_rotation, rotationOffsetVector);
	UpdateTransformDirection(rotationOffsetVector);
	m_matrixDirtied = true;
}

void Transform::Scale(DirectX::XMFLOAT3 a_scale)
{
	if (a_scale.x == 1 && a_scale.y == 1 && a_scale.z == 1) {
		return;
	}

	//Only update when offset not zero
	DirectX::XMVECTOR originalScale = DirectX::XMLoadFloat3(&m_scale);
	DirectX::XMVECTOR newScale = DirectX::XMLoadFloat3(&a_scale);
	newScale = DirectX::XMVectorMultiply(originalScale, newScale);
	
	DirectX::XMStoreFloat3(&m_position, newScale);
	m_matrixDirtied = true;
}

void Transform::Scale(float a_x, float a_y, float a_z)
{
	Transform::Scale(DirectX::XMFLOAT3(a_x, a_y, a_z));
}

void Transform::CalculateWorldMatrix()
{
	if (!m_matrixDirtied) return;

	DirectX::XMMATRIX translateMatrix = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_rotation));
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	DirectX::XMMATRIX world = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(scaleMatrix, rotationMatrix), translateMatrix);

	DirectX::XMStoreFloat4x4(&m_worldMatrix, world);
	DirectX::XMStoreFloat4x4(&m_worldInverseTransposeMatrix, 
		DirectX::XMMatrixInverse(0, DirectX::XMMatrixTranspose(world)));

	m_matrixDirtied = false;
}

void Transform::MoveRelative(DirectX::XMFLOAT3 a_offset)
{
	DirectX::XMVECTOR offsetVector = DirectX::XMLoadFloat3(&a_offset);
	
	DirectX::XMVECTOR currentRotation = DirectX::XMLoadFloat4(&m_rotation);
	
	DirectX::XMVECTOR moveOffsetVectorCorrected = DirectX::XMVector3Rotate(offsetVector, currentRotation);
	
	DirectX::XMVECTOR currentPosition = DirectX::XMLoadFloat3(&m_position);

	DirectX::XMVECTOR newPosition = DirectX::XMVectorAdd(moveOffsetVectorCorrected, currentPosition);

	DirectX::XMStoreFloat3(&m_position, newPosition);
	
	m_matrixDirtied = true;
}

void Transform::MoveRelative(float a_x, float a_y, float a_z)
{
	DirectX::XMFLOAT3 offset(a_x, a_y, a_z);
	Transform::MoveRelative(offset);
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	return m_right;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	return m_up;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	return m_forward;
}
void Transform::UpdateTransformDirection(DirectX::XMVECTOR a_rotation)
{
	DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&m_right);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_up);
	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&m_forward);

	right = DirectX::XMVector3Rotate(right, a_rotation);
	up = DirectX::XMVector3Rotate(up, a_rotation);
	forward = DirectX::XMVector3Rotate(forward, a_rotation);

	DirectX::XMStoreFloat3(&m_right, right);
	DirectX::XMStoreFloat3(&m_up, up);
	DirectX::XMStoreFloat3(&m_forward, forward);

}


void Transform::UpdateTransformDirection(DirectX::XMFLOAT4 a_rotation)
{
	DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&a_rotation);
	UpdateTransformDirection(rotation);
}


