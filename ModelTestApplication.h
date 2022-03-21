#pragma once
#include"DXUtils.h"
#include<DirectXMath.h>
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include<DirectXMath.h>
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
	Model m_planemodel;
	void InitPSO();
	D3D12_VIEWPORT aviewport;
	D3D12_RECT ascissorrect;
	XMMATRIX mvpmat;
};