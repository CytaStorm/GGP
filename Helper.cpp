#include "Helper.h"
#include <cmath>

DirectX::XMFLOAT3 Helper::QuaternionToEuler(DirectX::XMFLOAT4 a_quaternion)
{
	//from https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/Quaternions.pdf
	float x = a_quaternion.x;
	float y = a_quaternion.y;
	float z = a_quaternion.z;
	float w = a_quaternion.w;

	//x axis rotation
	float xRotation = atan2f(
		2 * (x * y - z * z),
		1 - 2 * (y * y + z * z));
	//y axis rotation
	float yRotation = asin(2 * (x * z + y * w));

	//z axis rotation
	float zRotation = atan2(
		2 * (x * w - y * z),
		1 - 2 * (y * y + w * w)
	);

	if (std::abs(yRotation) == std::numbers::pi_v<float> / 2) {
		zRotation = 0;
		y = atan2(y, x);
	}

	return DirectX::XMFLOAT3(xRotation, yRotation, zRotation);
}
