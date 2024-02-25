#include"BasicEntityRenderer.h"

BasicEntityRenderer::BasicEntityRenderer()
{
}

BasicEntityRenderer::~BasicEntityRenderer()
{
}
void BasicEntityRenderer::Render(DX12Commandlist& cmdlist, RenderData& renderData)
{
	for (std::shared_ptr<Entity> entity : m_entityList)
	{
		shared_ptr<BasicRenderableEntity> basicRenderableEntity = std::dynamic_pointer_cast<BasicRenderableEntity>(entity);
		m_mat.colour = basicRenderableEntity->GetBasicMaterialData().m_albedo;
		m_shadertransformconsts.model = basicRenderableEntity->GetModelMatrix();
		m_shadertransformconsts.mvp = DirectX::XMMatrixMultiply(m_shadertransformconsts.model, renderData.vpmat);
		cmdlist->SetGraphicsRoot32BitConstants(1, sizeof(m_shadertransformconsts) / 4, &m_shadertransformconsts, 0);
		cmdlist->SetGraphicsRoot32BitConstants(2, sizeof(m_mat) / 4, &m_mat, 0);
		basicRenderableEntity->Render(cmdlist);
		
	}
}
void BasicEntityRenderer::Init(ComPtr< ID3D12Device> creationdevice)
{

	PSOInitData basicpsodata = {};
	basicpsodata.type = GRAPHICS;

	//shaders to use
	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/shadowtest/VS.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/shadowtest/PS.hlsl", DX12Shader::ShaderType::PS);
	basicpsodata.m_shaderstouse.push_back(vs);
	basicpsodata.m_shaderstouse.push_back(ps);

	//build up graphic pso desc

	//shaderdesc
	basicpsodata.psodesc.graphicspsodesc.VS.pShaderBytecode = vs->GetCompiledCode();
	basicpsodata.psodesc.graphicspsodesc.VS.BytecodeLength = vs->GetCompiledCodeSize();
	basicpsodata.psodesc.graphicspsodesc.PS.pShaderBytecode = ps->GetCompiledCode();
	basicpsodata.psodesc.graphicspsodesc.PS.BytecodeLength = ps->GetCompiledCodeSize();

	//primitive setup
	basicpsodata.psodesc.graphicspsodesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//rasterizer setup
	basicpsodata.psodesc.graphicspsodesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	basicpsodata.psodesc.graphicspsodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	basicpsodata.psodesc.graphicspsodesc.RasterizerState.DepthClipEnable = FALSE;
	basicpsodata.psodesc.graphicspsodesc.RasterizerState.AntialiasedLineEnable = FALSE;

	//rtv&sample setup
	basicpsodata.psodesc.graphicspsodesc.SampleMask = UINT_MAX;
	basicpsodata.psodesc.graphicspsodesc.SampleDesc.Count = 1;
	basicpsodata.psodesc.graphicspsodesc.SampleDesc.Quality = 0;
	basicpsodata.psodesc.graphicspsodesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	basicpsodata.psodesc.graphicspsodesc.NumRenderTargets = 1;

	//raster state
	basicpsodata.psodesc.graphicspsodesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	basicpsodata.psodesc.graphicspsodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	basicpsodata.psodesc.graphicspsodesc.RasterizerState.DepthClipEnable = FALSE;
	basicpsodata.psodesc.graphicspsodesc.RasterizerState.AntialiasedLineEnable = FALSE;

	//root signature

	//1 root param for ps texture & sampler 
	D3D12_STATIC_SAMPLER_DESC simplesampler = {};
	simplesampler.Filter = D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	simplesampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	simplesampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	simplesampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	simplesampler.MipLODBias = 0;
	simplesampler.MaxAnisotropy = 0;
	simplesampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	simplesampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	simplesampler.MinLOD = 0.0f;
	simplesampler.MaxLOD = D3D12_FLOAT32_MAX;
	simplesampler.ShaderRegister = 0;
	simplesampler.RegisterSpace = 0;
	simplesampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_PARAMETER rootparam0 = {};

	D3D12_DESCRIPTOR_RANGE texsrvrange = {};
	texsrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	texsrvrange.NumDescriptors = 1;
	texsrvrange.BaseShaderRegister = 0;
	texsrvrange.RegisterSpace = 0;
	texsrvrange.OffsetInDescriptorsFromTableStart = 0;

	rootparam0.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam0.DescriptorTable.NumDescriptorRanges = 1;
	rootparam0.DescriptorTable.pDescriptorRanges = &texsrvrange;

	D3D12_ROOT_PARAMETER rootparam1 = {};
	rootparam1.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootparam1.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootparam1.Constants.Num32BitValues = sizeof(ShaderTransformConstants_General) / 4;
	rootparam1.Constants.RegisterSpace = 0;
	rootparam1.Constants.ShaderRegister = 0;
	D3D12_ROOT_PARAMETER rootparam2 = {};
	rootparam2.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootparam2.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
	rootparam2.Constants.Num32BitValues = sizeof(SimpleMaterial) / 4;
	rootparam2.Constants.RegisterSpace = 0;
	rootparam2.Constants.ShaderRegister = 1;
	vector<D3D12_ROOT_PARAMETER> rootparams;
	rootparams.push_back(rootparam0);
	rootparams.push_back(rootparam1);
	rootparams.push_back(rootparam2);

	vector<D3D12_STATIC_SAMPLER_DESC> staticsamplers;
	staticsamplers.push_back(simplesampler);
	basicpsodata.rootsignature.BuidDesc(rootparams, staticsamplers);

	//inputlayoutsetup
	{
		D3D12_INPUT_ELEMENT_DESC simplevsinputelementdesc[2] = {};
		simplevsinputelementdesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		simplevsinputelementdesc[0].InputSlot = 0;
		simplevsinputelementdesc[0].SemanticName = "POS";
		simplevsinputelementdesc[0].SemanticIndex = 0;
		simplevsinputelementdesc[0].InstanceDataStepRate = 0;
		simplevsinputelementdesc[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		simplevsinputelementdesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		simplevsinputelementdesc[1].InputSlot = 0;
		simplevsinputelementdesc[1].SemanticName = "VUV";
		simplevsinputelementdesc[1].SemanticIndex = 0;
		simplevsinputelementdesc[1].InstanceDataStepRate = 0;
		simplevsinputelementdesc[1].InstanceDataStepRate = 0;
		simplevsinputelementdesc[1].AlignedByteOffset = sizeof(float) * 3;//uv after 3 pos floats
		basicpsodata.psodesc.graphicspsodesc.InputLayout.NumElements = 2;
		basicpsodata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = simplevsinputelementdesc;
		basicpsodata.psodesc.graphicspsodesc.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;

	}



	//blendstate setup
	//blend state has fixed rt count of 8
	for (size_t i = 0; i < 8; i++)
	{
		//same for all 8 rtvs
		basicpsodata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendEnable = FALSE;
		basicpsodata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}


	m_basicpso.Init(creationdevice, basicpsodata);

}