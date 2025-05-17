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
		if (initdata.psodesc.computepsodesc.pRootSignature == nullptr)
		{
			m_initdata.rootsignature.Init(creationdevice);
			initdata.psodesc.computepsodesc.pRootSignature = m_initdata.rootsignature.GetRootSignature();
	}
		DXASSERT(creationdevice->CreateComputePipelineState(&initdata.psodesc.computepsodesc, IID_PPV_ARGS(m_pso.GetAddressOf())))
			break;

	case GRAPHICS:
		if (initdata.psodesc.graphicspsodesc.pRootSignature == nullptr)
		{
			m_initdata.rootsignature.Init(creationdevice);
			initdata.psodesc.graphicspsodesc.pRootSignature = m_initdata.rootsignature.GetRootSignature();
		}
	{DXASSERT(creationdevice->CreateGraphicsPipelineState(&initdata.psodesc.graphicspsodesc, IID_PPV_ARGS(m_pso.GetAddressOf())))
	
	break;
	}

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
	D3D12_GRAPHICS_PIPELINE_STATE_DESC nullgraphicsstate = { 0 };
	initdata.psodesc.graphicspsodesc = nullgraphicsstate;
	//primitive setup
	initdata.psodesc.graphicspsodesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//rasterizer setup
	initdata.psodesc.graphicspsodesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	initdata.psodesc.graphicspsodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	initdata.psodesc.graphicspsodesc.RasterizerState.DepthClipEnable = TRUE;
	initdata.psodesc.graphicspsodesc.RasterizerState.AntialiasedLineEnable = FALSE;
	initdata.psodesc.graphicspsodesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	initdata.psodesc.graphicspsodesc.RasterizerState.ForcedSampleCount = 0;

	//rtv&sample setup
	initdata.psodesc.graphicspsodesc.SampleMask = UINT_MAX;
	initdata.psodesc.graphicspsodesc.SampleDesc.Count = 1;
	initdata.psodesc.graphicspsodesc.SampleDesc.Quality = 0;
	initdata.psodesc.graphicspsodesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	initdata.psodesc.graphicspsodesc.NumRenderTargets = 1;

	//depth stencil state setup
	initdata.psodesc.graphicspsodesc.DepthStencilState.DepthEnable = TRUE;
	initdata.psodesc.graphicspsodesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	initdata.psodesc.graphicspsodesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
	initdata.psodesc.graphicspsodesc.DepthStencilState.StencilEnable = FALSE;
	initdata.psodesc.graphicspsodesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//blendstate setup
	//blend state has fixed rt count of 8
	for (size_t i = 0; i < 8; i++)
	{
		//same for all 8 rtvs
		initdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendEnable = FALSE;
		initdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	//input layout
	initdata.psodesc.graphicspsodesc.InputLayout.NumElements = 0;
	initdata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = nullptr;

}
void DX12PSO::SetPSOData_DefaultAlphaSettings(PSOInitData& psoinitdata, PSOBlendingSetting blendingSetting)
{
	//blend state has fixed rt count of 8
	for (size_t i = 0; i < 8; i++)
	{
		//same for all 8 rtvs
		switch (blendingSetting)
		{
		case PSOBlendingSetting::ADDITIVE_BLEND:
		{
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendEnable = TRUE;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND::D3D12_BLEND_DEST_COLOR;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_DEST_ALPHA;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND::D3D12_BLEND_ONE;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
			break;
		}
		case PSOBlendingSetting::PREMULTIPLIED_ALPHA:
		{
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendEnable = TRUE;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND::D3D12_BLEND_DEST_COLOR;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_DEST_ALPHA;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND::D3D12_BLEND_ONE;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
			break;
		}
		case PSOBlendingSetting::STRAIGHT_ALPHA:
		default:
		{
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendEnable = TRUE;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND::D3D12_BLEND_DEST_COLOR;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_DEST_ALPHA;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
			psoinitdata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
			break;
		}
		}
	}
}