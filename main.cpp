#include"windmaker.h"
#include"DX12Manager.h"
//#include"DX12SimpleTexturedQuadApplication.h"
#include"DX12SamplerfeedbackApplication.h"
//#include"DX12ComputeApplication.h"
#define ENABLEDEBUGLAYER true

typedef HRESULT(__stdcall* fPtr)(const IID&, void**);

//there are some live objects reporting at the termination of app,try to fix.
int main()
{
	//DX12ComputeApplication appmanager;
	DX12SamplerfeedbackApplication appmanager;
	//TexturedQuadApplication appmanager;
	WindMaker winmaker;
	DX12Manager dxmanager;
	dxmanager.Init(ENABLEDEBUGLAYER,&appmanager);
	
	winmaker.CreateWind(1280, 720, L"DXTest");
	appmanager.Initswapchain(dxmanager.GetDXGIFactory(), winmaker.GetWidth(), winmaker.GetHeight(), winmaker.Gethwnd());
	winmaker.RunMessageloop(&dxmanager);

	
	

	return 0;
}