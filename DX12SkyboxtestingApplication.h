#pragma once
#include"DXUtils.h"
#include<DirectXMath.h>
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include"DXCamera.h"
#include"DXTexManager.h"
using namespace DirectX;
class SkyboxTestApplication :public DX12ApplicationManagerBase
{
public:
	SkyboxTestApplication();
	~SkyboxTestApplication();
	void PreRenderUpdate()override;
	void Render()override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

protected:
	void InitExtras()override;

private:
	DXTexture m_skyboxtex;
	DX12PSO m_pso;
	DX12RootSignature m_rootsignature;
	Model m_planemodel;
	Model m_cubemodel;
	void InitPSO();
	DX12DESCHEAP m_resourceaccessheap;
};