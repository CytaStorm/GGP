#pragma once
#include <DirectXMath.h>
class Transform
{
public:
	DirectX::XMFLOAT3 m_right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 m_up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	DirectX::XMFLOAT3 m_forward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_worldInverseTransposeMatrix;

	/// <summary>
	/// Do we need to update the matrix?
	/// </summary>
	bool m_matrixDirtied = false;

	Transform();
	Transform(DirectX::XMFLOAT3 a_startingPosition, DirectX::XMFLOAT4 a_startingOrientation);
	~Transform();

	void SetPosition(float a_x, float a_y, float a_z);
	void SetPosition(DirectX::XMFLOAT3 a_position);
	void SetRotation(float a_x, float a_y, float a_z, float a_w);
	void SetRotation(DirectX::XMFLOAT4 a_rotation);
	void SetScale(float a_x, float a_y, float a_z);
	void SetScale(DirectX::XMFLOAT3 a_scale);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT4 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

	void MoveAbsolute(float a_x, float a_y, float a_z);
	void MoveAbsolute(DirectX::XMFLOAT3 a_offset);
	void Rotate(float a_pitch, float a_yaw, float a_roll);
	void Rotate(DirectX::XMFLOAT4 a_rotation);
	void Scale(float a_x, float a_y, float a_z);
	void Scale(DirectX::XMFLOAT3 a_scale);

	void CalculateWorldMatrix();

	//Cameras
	void MoveRelative(float a_x, float a_y, float a_z);
	void MoveRelative(DirectX::XMFLOAT3 a_offset);

	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();

	

private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT4 m_rotation;
	DirectX::XMFLOAT3 m_scale;

	/// <summary>
	/// Updates transform's right/up/forwards
	/// </summary>
	void UpdateTransformDirection(DirectX::XMVECTOR a_rotation);
	/// <summary>
	/// Updates transform's right/up/forwards
	/// </summary>
	void UpdateTransformDirection(DirectX::XMFLOAT4 a_rotation);
};

