#pragma once
#include"DXUtils.h"
#include<DirectXMath.h>
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include"DXCamera.h"
#include"DX12Texture.h"
#include"DX12DESCHEAP.h"
using namespace DirectX;
class MipTestApplication :public DX12ApplicationManagerBase
{
public:
	MipTestApplication();
	~MipTestApplication();
	void Update()override;
	void Render()override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

protected:
	void InitExtras()override;

private:
	DX12TextureSimple m_testtexture;
	DX12DESCHEAP m_srvheap, m_uavheap, m_uavheapupload;
	DX12PSO m_pso;
	DX12RootSignature m_rootsignature;
	Model m_planemodel;
	void InitPSO();
	DXCamera m_maincamera;
};