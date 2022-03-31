#include"windmaker.h"
#include"DX12Manager.h"
//#include"DX12SimpleTexturedQuadApplication.h"
//#include"DX12SamplerfeedbackApplication.h"
//#include"DX12ReservedResourceApplication.h"
#include"DX12SkyboxtestingApplication.h"
//#include"ModelTestApplication.h"
//#include"MipTestApplication.h"
//#include"DX12ComputeApplication.h"
#define ENABLEDEBUGLAYER true

typedef HRESULT(__stdcall* fPtr)(const IID&, void**);

//there are some live objects reporting at the termination of app,try to fix.
int main()
{
	//DX12ReservedResourceApplication appmanager;
	//DX12ComputeApplication appmanager;
	//DX12SamplerfeedbackApplication appmanager;
	//ModelTestApplication appmanager;
	//MipTestApplication appmanager;
	//TexturedQuadApplication appmanager;
	SkyboxTestApplication appmanager;
	WindowProcHook* windhook = dynamic_cast<WindowProcHook*>(&appmanager);
	assert(windhook != nullptr);//because all applicationd derive from base app which is derived from interface 
	
	WindMaker winmaker;
	DX12Manager dxmanager;
	dxmanager.Init(ENABLEDEBUGLAYER,&appmanager);
	WindProcManager::SetProcHook(windhook);
	winmaker.CreateWind(1280, 720, L"DXTest");
	appmanager.Initswapchain(dxmanager.GetDXGIFactory(), winmaker.GetWidth(), winmaker.GetHeight(), winmaker.Gethwnd());
	winmaker.RunMessageloop(&dxmanager);

	
	

	return 0;
}