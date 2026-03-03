#include"DXMathUtil.h"

using namespace DirectX;
DirectX::XMFLOAT3 DXMathUtil::AddFloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
	DirectX::XMFLOAT3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}
DirectX::XMFLOAT3 DXMathUtil::NormalizeFloat3(DirectX::XMFLOAT3 a)
{
	DirectX::XMVECTOR vec = XMLoadFloat3(&a);
	vec = XMVector3Normalize(vec);
	XMFLOAT3 result;
	XMStoreFloat3(&result, vec);
	return result;
}