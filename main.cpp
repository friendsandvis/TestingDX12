#include"windmaker.h"
#include"DX12Manager.h"
#define ENABLEDEBUGLAYER true


int main()
{
	DX12ApplicationManager drawrectappmanager;
	WindMaker winmaker;
	DX12Manager dxmanager;
	dxmanager.Init(ENABLEDEBUGLAYER,&drawrectappmanager);
	
	winmaker.CreateWind(1280, 720, L"DXTest");
	drawrectappmanager.Initswapchain(dxmanager.GetDXGIFactory(), winmaker.GetWidth(), winmaker.GetHeight(), winmaker.Gethwnd());
	winmaker.RunMessageloop(&dxmanager);

	return 0;
}