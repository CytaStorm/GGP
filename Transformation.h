#pragma once
#include <DirectXMath.h>
class Transformation
{
private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT4 m_rotation;
	DirectX::XMFLOAT3 m_scale;


	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_worldInverseTransposeMatrix;

	/// <summary>
	/// Do we need to update the matrix?
	/// </summary>
	bool m_dirtied = false;
public:
	Transformation();
	~Transformation();

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
};

