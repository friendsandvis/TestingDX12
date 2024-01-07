#pragma once
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"AssetManager.h"
#include"DXTexManager.h"
#include"DX12Texture.h"
#include"CameraMovementManager.h"


class ShadowTestApplication :public DX12ApplicationManagerBase
{
public:
	ShadowTestApplication();
	~ShadowTestApplication();
	void PreRenderUpdate()override;
	void Render()override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;


protected:
	void InitExtras();

private:
	struct SimpleMaterial
	{
		XMFLOAT4 colour;
	};
	DX12PSO m_basicpso;
	ComPtr<ID3DBlob> m_emptyrootsignatureblob, m_rootsignatureerrors;
	Model m_planemodel,m_cubemodel;
	DXTexture m_redtexture;
	DX12DESCHEAP m_resaccessviewdescheap;//CBV/SRV/UAV
	DXFPSCameraController m_maincameracontroller;
	ShaderTransformConstants_General m_shadertransformconsts;
	SimpleMaterial m_mat;

	void InitBasicPSO();

};


