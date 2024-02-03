#include"DX12ShadowTestApplication.h"
#include"DXUtils.h"


ShadowTestApplication::ShadowTestApplication()
	:m_basicCubeEntity(MODELTYPE::BASIC_CUBE),
	m_basicPlaneEntity(MODELTYPE::BASIC_PLANE)
{
	m_maincameracontroller.SetCameratoControl(&m_maincamera);
}

ShadowTestApplication::~ShadowTestApplication()
{
}

void ShadowTestApplication::InitExtras()
{
	//init pso
	InitBasicPSO();

	//init desc heaps
	D3D12_DESCRIPTOR_HEAP_DESC heapdesc = {};
	heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapdesc.NumDescriptors = 1;	//just creating for an srv
	heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	m_resaccessviewdescheap.Init(heapdesc, m_creationdevice);


	//init assets
	m_basicCubeEntity.Init(m_creationdevice, m_uploadcommandlist);
	m_basicCubeEntity.SetName("Cube0");
	m_basicCubeEntity.SetTransformationData(0.5f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), 0.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));

	m_basicPlaneEntity.Init(m_creationdevice, m_uploadcommandlist);
	m_basicPlaneEntity.SetName("Plane0");
	m_basicPlaneEntity.SetTransformationData(2.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), 90.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	DXTexManager::LoadTexture(L"textures/texlargemiped.dds", m_redtexture.GetDXImageData());
	bool initsuccess = m_redtexture.Init(m_creationdevice);
	m_redtexture.SetName(L"REDTEX");
	
	
	
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC redtexsrvdesc = {};
		redtexsrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		redtexsrvdesc.Texture2D.MipLevels = (UINT)m_redtexture.GetTotalMipCount();
		redtexsrvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;


		m_redtexture.CreateSRV(m_creationdevice, redtexsrvdesc, m_resaccessviewdescheap.GetCPUHandlefromstart());
	}

	//upload asset data
	m_uploadcommandlist.Reset();
	m_redtexture.UploadTexture(m_uploadcommandlist);
	DXASSERT(m_uploadcommandlist->Close())

}

void ShadowTestApplication::InitBasicPSO()
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


	m_basicpso.Init(m_creationdevice, basicpsodata);
}


void ShadowTestApplication::Render()
{
	m_primarycmdlist.Reset();
	m_primarycmdlist->SetPipelineState(m_basicpso.GetPSO());
	//m_primarycmdlist->SetGraphicsRootSignature(m_emptyrootsignature.Get());
	m_primarycmdlist->SetGraphicsRootSignature(m_basicpso.GetRootSignature());
	ID3D12DescriptorHeap* descheapstoset[1];
	descheapstoset[0] = m_resaccessviewdescheap.GetDescHeap();
	m_primarycmdlist->SetDescriptorHeaps(1, descheapstoset);
	m_primarycmdlist->SetGraphicsRootDescriptorTable(0, m_resaccessviewdescheap.GetGPUHandlefromstart());

	{
		D3D12_VIEWPORT aviewport = {};
		aviewport.TopLeftX = 0;
		aviewport.TopLeftY = 0;
		aviewport.Width = m_swapchain.GetSwapchainWidth();
		aviewport.Height = m_swapchain.GetSwapchainHeight();
		aviewport.MinDepth = 0.0f;
		aviewport.MaxDepth = 1.0f;

		D3D12_RECT ascissorrect = {};
		ascissorrect.left = 0;
		ascissorrect.top = 0;
		ascissorrect.right = aviewport.Width;
		ascissorrect.bottom = aviewport.Height;

		m_primarycmdlist->RSSetViewports(1, &aviewport);
		m_primarycmdlist->RSSetScissorRects(1, &ascissorrect);
	}

	m_primarycmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D12_VERTEX_BUFFER_VIEW vbview;
		D3D12_INDEX_BUFFER_VIEW Ibview;


	//set rtv
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_swapchain.GetCurrentbackbufferIndex());

	D3D12_RESOURCE_BARRIER backbufferbarrier = {};
	backbufferbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	backbufferbarrier.Transition.pResource = m_swapchain.GetBackBuffer(m_swapchain.GetCurrentbackbufferIndex());
	backbufferbarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	backbufferbarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	backbufferbarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_primarycmdlist->ResourceBarrier(1, &backbufferbarrier);
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE, nullptr);
	float rtclearcolour[4] = { 1.0f,1.0f,1.0f,1.0f };
	m_primarycmdlist->ClearRenderTargetView(rtvhandle, rtclearcolour, 0, nullptr);
	XMMATRIX vpmat = m_maincamera.GetVP();
	
	
	//draw plane model
	 {
		m_mat.colour = { 1.0f,0.0f,0.0f,1.0f };
		m_shadertransformconsts.model = m_basicPlaneEntity.GetModelMatrix();
		m_shadertransformconsts.mvp = DirectX::XMMatrixMultiply(m_shadertransformconsts.model, vpmat);
		m_primarycmdlist->SetGraphicsRoot32BitConstants(1, sizeof(m_shadertransformconsts) / 4, &m_shadertransformconsts, 0);
		m_primarycmdlist->SetGraphicsRoot32BitConstants(2, sizeof(m_mat) / 4, &m_mat, 0);
		m_basicPlaneEntity.Render(m_primarycmdlist);
	}
	//draw cube model
	 {
		
		m_mat.colour = { 0.0f,1.0f,0.0f,1.0f };
		m_shadertransformconsts.model = m_basicCubeEntity.GetModelMatrix();
		m_shadertransformconsts.mvp = DirectX::XMMatrixMultiply(m_shadertransformconsts.model, vpmat);
		m_primarycmdlist->SetGraphicsRoot32BitConstants(1, sizeof(m_shadertransformconsts) / 4, &m_shadertransformconsts, 0);
		m_primarycmdlist->SetGraphicsRoot32BitConstants(2, sizeof(m_mat) / 4, &m_mat, 0);

		m_basicCubeEntity.Render(m_primarycmdlist);
	 }
	backbufferbarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	backbufferbarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_primarycmdlist->ResourceBarrier(1, &backbufferbarrier);

	DXASSERT(m_primarycmdlist->Close())


		BasicRender();
}
void ShadowTestApplication::PreRenderUpdate()
{
	m_maincameracontroller.Update();
}
void ShadowTestApplication::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	m_maincameracontroller.ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);

}


