#pragma once
#include <DirectXMath.h>
#include <numbers>
struct Helper
{
	static DirectX::XMFLOAT3 QuaternionToEuler(DirectX::XMFLOAT4 a_quaternion);
};

