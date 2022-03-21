#pragma once
#include"DXUtils.h"
#include<DirectXMath.h>
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
using namespace DirectX;
class ModelTestApplication :public DX12ApplicationManagerBase
{
public:
	ModelTestApplication();
	~ModelTestApplication();
	void Render()override;

protected:
	void InitExtras()override;

private:
	DX12PSO m_pso;
	DX12RootSignature m_rootsignature;
	void InitPSO();
};