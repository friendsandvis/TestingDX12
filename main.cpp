#include"windmaker.h"
#include"DX12Manager.h"
//#include"DX12SimpleTexturedQuadApplication.h"
#include"DX12SamplerfeedbackApplication.h"
//#include"DX12ReservedResourceApplication.h"
//#include"DX12SkyboxtestingApplication.h"
#include"ModelTestApplication.h"
//#include"MipTestApplication.h"
//#include"DX12ComputeApplication.h"
#include"RayTracingTestApplication.h"
#include<iostream>
#define ENABLEDEBUGLAYER true

typedef HRESULT(__stdcall* fPtr)(const IID&, void**);

//there are some live objects reporting at the termination of app,try to fix.
int main()
{
	//multiple applications present in this testing program choose the one to run(via user input).
	std::cout << "1.RaytracingApplication.\n";
	std::cout << "2.SamplerFeedbackApplication.\n";
	std::cout << "3.ModelTestApplication.\n";
	std::cout << "Enter application number  to run:\n";
	int appnumber;
	std::cin >> appnumber;
	DX12ApplicationManagerBase* appmanager = nullptr;
	switch (appnumber)
	{
	case 1:
		appmanager = new RayTracingApplication(); break;
	case 2:
		appmanager = new DX12SamplerfeedbackApplication(); break;
	case 3:
		appmanager = new ModelTestApplication(); break;
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
	winmaker.RunMessageloop(&dxmanager);

	
	if (appmanager)
	{
		delete appmanager;
	}

	return 0;
}