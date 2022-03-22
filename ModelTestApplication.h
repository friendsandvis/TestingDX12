#pragma once
#include"DXUtils.h"
#include<DirectXMath.h>
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include"DXCamera.h"
using namespace DirectX;
class ModelTestApplication :public DX12ApplicationManagerBase
{
public:
	ModelTestApplication();
	~ModelTestApplication();
	void Update()override;
	void Render()override;

protected:
	void InitExtras()override;

private:
	DX12PSO m_pso;
	DX12RootSignature m_rootsignature;
	Model m_planemodel;
	void InitPSO();
	DXCamera m_maincamera;
};