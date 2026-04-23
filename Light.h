#pragma once
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

#include <DirectXMath.h>

struct Light
{
	int m_Type;
	DirectX::XMFLOAT3 m_Direction; //16

	float m_Range;
	DirectX::XMFLOAT3 m_Position; //16

	float m_Intensity;
	DirectX::XMFLOAT3 m_Color; //16

	float m_SpotInnerAngle;
	float m_SpotOuterAngle;
};

