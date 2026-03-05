#pragma once
#include <DirectXMath.h>
class Transform
{
public:
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

	const DirectX::XMFLOAT3& GetPosition();
	const DirectX::XMFLOAT4& GetRotation();
	const DirectX::XMFLOAT3& GetScale();
	const DirectX::XMFLOAT4X4& GetWorldMatrix();
	const DirectX::XMFLOAT4X4& GetWorldInverseTransposeMatrix();

	void MoveAbsolute(float a_x, float a_y, float a_z);
	void MoveAbsolute(DirectX::XMFLOAT3 a_offset);
	void Rotate(float a_pitch, float a_yaw, float a_roll);
	void Rotate(DirectX::XMFLOAT4 a_rotationDelta);
	void Scale(float a_x, float a_y, float a_z);
	void Scale(DirectX::XMFLOAT3 a_scale);

	void CalculateWorldMatrix();

	//Cameras
	void MoveRelative(float a_x, float a_y, float a_z);
	void MoveRelative(DirectX::XMFLOAT3 a_offset);

	const DirectX::XMFLOAT3& GetRight();
	const DirectX::XMFLOAT3& GetUp();
	const DirectX::XMFLOAT3& GetForward();

private:
	DirectX::XMFLOAT3 m_right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 m_up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	DirectX::XMFLOAT3 m_forward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_worldInverseTransposeMatrix;

	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT4 m_rotation;
	DirectX::XMFLOAT3 m_scale;

	/// <summary>
	/// Updates transform forward/right/up
	/// </summary>
	/// <param name="a_rotationDelta">Rotation offset (from original rotation</param>
	void UpdateTransformDirection(DirectX::XMFLOAT4 a_rotationDelta);
};

