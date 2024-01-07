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