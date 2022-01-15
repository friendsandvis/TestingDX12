#include"DX12Manager.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")



DX12Manager::DX12Manager()
{
	//DXASSERT(CreateDXGIFactory2(0, IID_PPV_ARGS(m_dxgifactory.GetAddressOf())))
	
}

DX12Manager::~DX12Manager()
{
}

void DX12Manager::Init(bool enabledebuglayer)
{

}