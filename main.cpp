#include"windmaker.h"

int main()
{

	WindMaker winmaker;
	winmaker.CreateWind(1280, 720, L"DXTest");
	winmaker.RunMessageloop();

	return 0;
}