#include"windmaker.h"
#include"DX12Manager.h"

int main()
{

	WindMaker winmaker;
	DX12Manager dxmanager;
	winmaker.CreateWind(1280, 720, L"DXTest");
	winmaker.RunMessageloop();

	return 0;
}