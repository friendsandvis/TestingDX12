#include"windmaker.h"
#include"DX12Manager.h"
//#include"DX12SimpleTexturedQuadApplication.h"
//#include"DX12SamplerfeedbackApplication.h"
//#include"DX12ReservedResourceApplication.h"
#include"DX12SkyboxtestingApplication.h"
//#include"ModelTestApplication.h"
//#include"MipTestApplication.h"
//#include"DX12ComputeApplication.h"
//#include"RayTracingTestApplication.h"
#define ENABLEDEBUGLAYER true

typedef HRESULT(__stdcall* fPtr)(const IID&, void**);

//there are some live objects reporting at the termination of app,try to fix.
int main()
{
	//DX12ReservedResourceApplication appmanager;
	//DX12ComputeApplication appmanager;
	//DX12SamplerfeedbackApplication appmanager;
	//ModelTestApplication appmanager;
	//RayTracingApplication appmanager;
	//MipTestApplication appmanager;
	//TexturedQuadApplication appmanager;
	SkyboxTestApplication appmanager;
	WindowProcHook* windhook = dynamic_cast<WindowProcHook*>(&appmanager);
	WindMaker winmaker;
	DX12Manager dxmanager;
	winmaker.CreateWind(1920, 1080, L"DXTest");
	assert(windhook != nullptr);//because all applicationd derive from base app which is derived from interface
	WindProcManager::SetProcHook(windhook);
	
	
	
	
	
	
	dxmanager.Init(winmaker.GetWidth(), winmaker.GetHeight(), winmaker.Gethwnd(), ENABLEDEBUGLAYER, &appmanager);
	winmaker.RunMessageloop(&dxmanager);

	
	

	return 0;
}