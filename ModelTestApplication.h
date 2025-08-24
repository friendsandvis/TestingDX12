#pragma once
#include"DXUtils.h"
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include"DXCamera.h"
#include"CameraMovementManager.h"
using namespace DirectX;
class ModelTestApplication :public DX12ApplicationManagerBase
{
public:
	ModelTestApplication();
	~ModelTestApplication();
	void PreRenderUpdate()override;
	void Render()override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

protected:
	void InitExtras()override;

private:
	struct GeneralConstants
	{
		unsigned int usematerialtextures;
	};
	struct CustomMaterial
	{
		float usecustomMaterial = 0.0f;
		float paddingValue = 0.0f;
		float specularValue = 32.0f;
		unsigned int lightingMode = static_cast<unsigned int>(LIGHTINGMODE::COMPLETELIGHTING_BASIC);
		XMFLOAT4 viewPos;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
	};
	struct TestLight
	{
		XMFLOAT3 lightCol;
		float data1;
		XMFLOAT3 lightPos;
		float data2;
	};
	DX12PSO m_pso,m_pso_alphablending;
	Model m_planemodel,m_trianglemodel;
	Model m_cubemodel;
	Model m_loadedmodel;
	CompoundModel m_loadedcompoundmodel;
	DXFPSCameraController m_maincameracontroller;
	void InitPSO();
};