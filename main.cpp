#include"windmaker.h"
#include"DX12Manager.h"
#include"DX12SimpleTexturedQuadApplication.h"
#include"DX12SamplerfeedbackApplication.h"
//#include"DX12ReservedResourceApplication.h"
#include"DX12SkyboxtestingApplication.h"
#include"ModelTestApplication.h"
//#include"MipTestApplication.h"
//#include"DX12ComputeApplication.h"
#include"RayTracingTestApplication.h"
#include"RayTracingTestApplication2.h"
#include"RayTracingTestApplication_Advanced.h"
#include"RayTracingTestApplication_AdvancedAOTest.h"
#include"DX12ShadowTestApplication.h"
#include<iostream>
#include"StreamableTextureFileCreator.h"
#include"StreamableTextureFileReader.h"
#include"DX12BlendingTestApplication.h"
//#define TESTBLENDINGAPP
#ifdef _DEBUG
#define ENABLEDEBUGLAYER true
#else
#define ENABLEDEBUGLAYER false
#endif // _DEBUG
//launch option 6 if used for skybox application if this maco is defined else it is a free slot used for otther needed utils(currently for stf creation)
#define ALLOWSKYBOXTESTAPP 1


typedef HRESULT(__stdcall* fPtr)(const IID&, void**);

//there are some live objects reporting at the termination of app,try to fix.
int main()
{
	//multiple applications present in this testing program choose the one to run(via user input).
	std::cout << "0.texquadApplication.\n";
	std::cout << "1.RaytracingApplication.\n";
	std::cout << "2.RaytracingApplication2.\n";
	std::cout << "3.ModelTestApplication.\n";
	std::cout << "4.RaytracingApplicationAdvanced.\n";
	std::cout << "5.SamplerFeedbackApplication.\n";
#ifdef ALLOWSKYBOXTESTAPP
	std::cout << "6.skyboxtestApplication.\n";
#else
	std::cout << "6.run stf creator.\n";
#endif // ALLOWSKYBOXTESTAPP

	std::cout << "7.run stf reader.\n";
	std::cout << "8.RaytracingApplicationAdvancedAOTest.\n";
#ifdef TESTBLENDINGAPP
	std::cout << "9.blendingtestApplication.\n";
#else
	std::cout << "9.shadowtestApplication.\n";
#endif // TESTBLENDINGAPP
	std::cout << "Enter application number  to run:\n";
	int appnumber;
	std::cin >> appnumber;
	DX12ApplicationManagerBase* appmanager = nullptr;
	switch (appnumber)
	{
	case 0:
		appmanager = new TexturedQuadApplication(); break;
	case 1:
		appmanager = new RayTracingApplication(); break;
	case 2:
		appmanager = new RayTracingApplication2(); break;
	case 3:
		appmanager = new ModelTestApplication(); break;
	case 4:
		appmanager = new RayTracingApplicationAdvanced(); break;
	case 5:
		appmanager = new DX12SamplerfeedbackApplication(); break;
	case 6:
#ifdef ALLOWSKYBOXTESTAPP
		{
			appmanager = new SkyboxTestApplication(); break;
	}
#else
	{
		StreamableTextureFileCreator stfcreator;
		int res = stfcreator.ExportToSTF(L"textures/texlargemiped.dds", "textures/stf/texlargemiped.stf");
		assert(res != 0);
		break;
	}
#endif // ALLOWSKYBOXTESTAPP

	
	case 7:
	{
		StreamableTextureFileReader stfreader;
		int res=stfreader.Init("textures/stf/texlargemiped.stf");
		stfreader.PrepSubResPixData();
		break;
	}
	case 8:
		appmanager = new RayTracingTestApplication_AdvancedAOTest(); break;
	case 9:
#ifdef TESTBLENDINGAPP
		appmanager = new BlendingTestApplication(); break;
#else
		appmanager = new ShadowTestApplication(); break;
#endif // TESTBLENDINGAPP
	default:
		std::cout << "Invalid App number choosen";
		return 1;
	}
	/*DX12ReservedResourceApplication appmanager;
	DX12ComputeApplication appmanager;
	ModelTestApplication appmanager;
	MipTestApplication appmanager;
	TexturedQuadApplication appmanager;
	SkyboxTestApplication appmanager;*/
	WindowProcHook* windhook = dynamic_cast<WindowProcHook*>(appmanager);
	WindMaker winmaker;
	DX12Manager dxmanager;
	winmaker.CreateWind(1920, 1080, L"DXTest");
	assert(windhook != nullptr);//because all applicationd derive from base app which is derived from interface
	WindProcManager::SetProcHook(windhook);
	
	
	
	
	
	
	dxmanager.Init(winmaker.GetWidth(), winmaker.GetHeight(), winmaker.Gethwnd(), ENABLEDEBUGLAYER, appmanager);
	winmaker.RunMessageloop(&dxmanager, appmanager->IsIMGUIAllowed());

	
	if (appmanager)
	{
		appmanager->Destroy();
		delete appmanager;
	}

	return 0;
}