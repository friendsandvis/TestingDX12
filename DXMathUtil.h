#pragma once
#include<DirectXMath.h>
class DXMathUtil
{
public:
	static DirectX::XMFLOAT3 AddFloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);
	static DirectX::XMFLOAT3 NormalizeFloat3(DirectX::XMFLOAT3 a);
};