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

void DX12PSO::DefaultInitPSOData(PSOInitData& initdata)
{
	if (initdata.type != PSOType::GRAPHICS)
	{
		//not supported yet
		return;
	}
	//primitive setup
	initdata.psodesc.graphicspsodesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//rasterizer setup
	initdata.psodesc.graphicspsodesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	initdata.psodesc.graphicspsodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	initdata.psodesc.graphicspsodesc.RasterizerState.DepthClipEnable = FALSE;
	initdata.psodesc.graphicspsodesc.RasterizerState.AntialiasedLineEnable = FALSE;

	//rtv&sample setup
	initdata.psodesc.graphicspsodesc.SampleMask = UINT_MAX;
	initdata.psodesc.graphicspsodesc.SampleDesc.Count = 1;
	initdata.psodesc.graphicspsodesc.SampleDesc.Quality = 0;
	initdata.psodesc.graphicspsodesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	initdata.psodesc.graphicspsodesc.NumRenderTargets = 1;

	//blendstate setup
	//blend state has fixed rt count of 8
	for (size_t i = 0; i < 8; i++)
	{
		//same for all 8 rtvs
		initdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendEnable = FALSE;
		initdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
}