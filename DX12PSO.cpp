#include"DX12PSO.h"

DX12PSO::DX12PSO()
{

}

DX12PSO::~DX12PSO()
{
	//delete all the shaders being used(they are expected to be dynamically allocated by user(might shift it to the 
	for (size_t i = 0; i < m_initdata.m_shaderstouse.size(); i++)
	{
		if (m_initdata.m_shaderstouse[i])
		{
			delete m_initdata.m_shaderstouse[i];
		}
	}
}


void DX12PSO::Init(ComPtr< ID3D12Device> creationdevice, PSOInitData initdata)
{
	m_initdata = initdata;

	switch (m_initdata.type)
	{
	case COMPUTE:
		DXASSERT(creationdevice->CreateComputePipelineState(&initdata.psodesc.computepsodesc, IID_PPV_ARGS(m_pso.GetAddressOf())))
			break;

	case GRAPHICS:
		DXASSERT(creationdevice->CreateGraphicsPipelineState(&initdata.psodesc.graphicspsodesc, IID_PPV_ARGS(m_pso.GetAddressOf())))
			break;

	default:
		break;
	}
}