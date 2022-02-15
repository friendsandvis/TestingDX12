#include"windmaker.h"
#include"DX12Manager.h"
#include"DX12SimpleTexturedQuadApplication.h"
#include<dxgidebug.h>
#define ENABLEDEBUGLAYER true

typedef HRESULT(__stdcall* fPtr)(const IID&, void**);

//there are some live objects reporting at the termination of app,try to fix.
int main()
{
	TexturedQuadApplication drawrectappmanager;
	WindMaker winmaker;
	DX12Manager dxmanager;
	dxmanager.Init(ENABLEDEBUGLAYER,&drawrectappmanager);
	
	winmaker.CreateWind(1280, 720, L"DXTest");
	drawrectappmanager.Initswapchain(dxmanager.GetDXGIFactory(), winmaker.GetWidth(), winmaker.GetHeight(), winmaker.Gethwnd());
	winmaker.RunMessageloop(&dxmanager);

	
	

	return 0;
}