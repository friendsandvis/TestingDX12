#include"DXUtils.h"
#pragma comment(lib,"DirectX-Guids.lib")

DXUtils::DXUtils()
{
}

DXUtils::~DXUtils()
{
}

bool DXUtils::IsBarrierSafeToExecute(const D3D12_RESOURCE_BARRIER barrier)
{
	//before and after states of transition must be diffrent.
	bool issafe = false;

	if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
	{
		issafe = (barrier.Transition.StateAfter != barrier.Transition.StateBefore);
	}

	return issafe;
}
DirectX::XMMATRIX DXUtils::GetTransformationMatrix(float uniformscale, DirectX::XMVECTOR rotationaxis, float rotationangle, DirectX::XMVECTOR translate)
{
	DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX scaleingmat = DirectX::XMMatrixScaling(uniformscale, uniformscale, uniformscale);
	DirectX::XMMATRIX rotationmat = DirectX::XMMatrixRotationAxis(rotationaxis,DirectX::XMConvertToRadians(rotationangle));
	DirectX::XMMATRIX translationmat = DirectX::XMMatrixTranslationFromVector(translate);
	transform = DirectX::XMMatrixMultiply(scaleingmat, rotationmat);
	transform = DirectX::XMMatrixMultiply(transform, translationmat);
	return transform;
}
DirectX::XMMATRIX DXUtils::GetTransformationMatrix(DirectX::XMVECTOR scale, DirectX::XMVECTOR rotationaxis, float rotationangle, DirectX::XMVECTOR translate)
{
	DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX scaleingmat = DirectX::XMMatrixScalingFromVector(scale);
	DirectX::XMMATRIX rotationmat = DirectX::XMMatrixRotationAxis(rotationaxis, DirectX::XMConvertToRadians(rotationangle));
	DirectX::XMMATRIX translationmat = DirectX::XMMatrixTranslationFromVector(translate);
	transform = DirectX::XMMatrixMultiply(scaleingmat, rotationmat);
	transform = DirectX::XMMatrixMultiply(transform, translationmat);
	return transform;
}
DirectX::XMVECTOR DXUtils::Float4tovector(DirectX::XMFLOAT4 value)
{
	return (DirectX::XMVectorSet(value.x, value.y, value.z, value.w));
}
void DXUtils::GetIdxSuffix(unsigned idx, wstring& idxsuffix)
{
	idxsuffix = L"";
	switch (idx)
	{
	case 0:
	{
		idxsuffix = L"0";
		break;
	}
	case 1:
	{
		idxsuffix = L"1";
		break;
	}
	case 2:
	{
		idxsuffix = L"2";
		break;
	}
	case 3:
	{
		idxsuffix = L"3";
		break;
	}
	default:
		break;
	}
}
void DXUtils::GetIdxSuffix(unsigned idx, string& idxsuffix)
{
	idxsuffix = "";
	switch (idx)
	{
	case 0:
	{
		idxsuffix = "0";
		break;
	}
	case 1:
	{
		idxsuffix = "1";
		break;
	}
	case 2:
	{
		idxsuffix = "2";
		break;
	}
	default:
		break;
	}
}