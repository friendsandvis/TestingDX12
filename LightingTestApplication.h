#pragma once
#include"DXUtils.h"
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"DX12RootSignature.h"
#include"AssetManager.h"
#include"DXCamera.h"
#include"CameraMovementManager.h"
using namespace DirectX;
//#define TESTLIGHTTYPE_POINT
#define TESTLIGHTTYPE_DIRECTION
class LightingTestApplication :public DX12ApplicationManagerBase
{
public:
	LightingTestApplication();
	~LightingTestApplication();
	void PreRenderUpdate()override;
	void Render()override;
	void IMGUIRenderAdditional() override;
	void ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

protected:
	void InitExtras()override;

private:
	struct GeneralConstants
	{
		unsigned int usematerialtextures;
	};
	/*
	* when using materialtextures then ambient,diffuse & specular represent the factor to multiply in correcponding part's calculation;
	* normally it represents the resopective material peroperty.
	*/
	struct CustomMaterial
	{
		float usecustomMaterial = 0.0f;
		float useMaterialTextures = 0.0f;
		float specularValue = 32.0f;
		unsigned int lightingMode = static_cast<unsigned int>(LIGHTINGMODE::COMPLETELIGHTING_BASIC);
		XMFLOAT4 viewPos;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
	};
#ifdef TESTLIGHTTYPE_POINT
	struct TestLight
	{
		XMFLOAT3 lightCol;
		float data1;
		XMFLOAT3 lightPos;
		float data2;
	};
#elif defined(TESTLIGHTTYPE_DIRECTION)
	struct TestLight
	{
		XMFLOAT3 lightCol;
		float data1;
		XMFLOAT3 lightDir;
		float data2;
	};
#endif // TESTLIGHTTYPE_POINT
	DX12PSO m_pso, m_pso_alphablending;
	Model m_planemodel, m_trianglemodel;
	Model m_cubemodel_simpleTesting;
	Model m_cubemodel_simpleLight;
	Model m_loadedmodel;
	CompoundModel m_loadedcompoundmodel;
	DXFPSCameraController m_maincameracontroller;
	bool m_Imgui_mousecontrol_camera;
	TestLight m_TestLightProperties;
	DX12Buffer m_materialTexDatabuffer;
	DX12DESCHEAP m_texdataResourceviewheap;
	DXTexture* m_boxtextureDiffuse;
	DXTexture* m_boxtextureSpec;
	MaterialDataGPU m_boxMaterialGPUData;
	void InitPSO();
};