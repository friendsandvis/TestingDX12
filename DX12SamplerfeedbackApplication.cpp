#include"DX12SamplerfeedbackApplication.h"
#include"DXUtils.h"


DX12SamplerfeedbackApplication::DX12SamplerfeedbackApplication()
	:m_planemodel(ModelDataUploadMode::COPY)
{
}

DX12SamplerfeedbackApplication::~DX12SamplerfeedbackApplication()
{
}

void DX12SamplerfeedbackApplication::InitExtras()
{
	//check sampler feedback support
	D3D12_FEATURE_DATA_D3D12_OPTIONS7 option7features = {};
	DXASSERT(m_creationdevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &option7features, sizeof(option7features)))
		assert(option7features.SamplerFeedbackTier != D3D12_SAMPLER_FEEDBACK_TIER_NOT_SUPPORTED);

	//init pso
	InitBasicPSO();

	//init desc heaps
	D3D12_DESCRIPTOR_HEAP_DESC heapdesc = {};
	heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapdesc.NumDescriptors = 2;	//just creating for a srv and then a uav
	heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	m_resaccessviewdescheap.Init(heapdesc, m_creationdevice);


	//init assets
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	DXTexManager::LoadTexture(L"textures/tex3_miped.dds", m_redtexture.GetDXImageData());
	bool initsuccess = m_redtexture.Init(m_creationdevice);
	m_redtexture.SetName(L"GREENTEX");

	{
		DXImageData& redteximgdata = m_redtexture.GetDXImageData();
		samplerFeedbackTexInitData sftexinitdata = {};
		sftexinitdata.heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
		sftexinitdata.heapprops.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		sftexinitdata.heapprops.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		sftexinitdata.heapflags = D3D12_HEAP_FLAG_NONE;
		sftexinitdata.resdesc.Alignment = 0;
		sftexinitdata.resdesc.DepthOrArraySize = redteximgdata.m_imagemetadata.depth;
		sftexinitdata.resdesc.Width = redteximgdata.m_imagemetadata.width;
		sftexinitdata.resdesc.Height = redteximgdata.m_imagemetadata.height;
		sftexinitdata.resdesc.MipLevels = 1;
		sftexinitdata.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		sftexinitdata.resdesc.Format = DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE;
		sftexinitdata.resdesc.SampleDesc.Count = 1;
		sftexinitdata.resdesc.SampleDesc.Quality = 0;
		sftexinitdata.resdesc.SamplerFeedbackMipRegion.Depth = 1;
		sftexinitdata.resdesc.SamplerFeedbackMipRegion.Width = 4;
		sftexinitdata.resdesc.SamplerFeedbackMipRegion.Height = 4;
		sftexinitdata.resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		sftexinitdata.resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		ComPtr<ID3D12Device8> device8;
		DXASSERT(m_creationdevice.As(&device8))

		m_redtexfeedbackunit.m_feedbacktex.Init(device8, sftexinitdata);
		m_redtexfeedbackunit.m_feedbacktex.Pair(device8,&m_redtexture, m_resaccessviewdescheap.GetCPUHandleOffseted(1));
	}

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC redtexsrvdesc = {};
		redtexsrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		redtexsrvdesc.Texture2D.MipLevels = (UINT)m_redtexture.GetTotalMipCount();
		redtexsrvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;


		m_redtexture.CreateSRV(m_creationdevice, redtexsrvdesc, m_resaccessviewdescheap.GetCPUHandleOffseted(0));
	}

	//upload asset data
	m_uploadcommandlist.Reset();
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_redtexture.UploadTexture(m_uploadcommandlist);
	DXASSERT(m_uploadcommandlist->Close())

}

void DX12SamplerfeedbackApplication::InitBasicPSO()
{
	PSOInitData basicpsodata = {};
	basicpsodata.type = GRAPHICS;

	//shaders to use
	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/BasicVertexShader_1.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/PixelShader_SF.hlsl", DX12Shader::ShaderType::PS);
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
	simplesampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	simplesampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	simplesampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	simplesampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	simplesampler.MipLODBias = 0;
	simplesampler.MaxAnisotropy = 0;
	simplesampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	simplesampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	simplesampler.MinLOD = 0.0f;
	simplesampler.MaxLOD = D3D12_FLOAT32_MAX;
	simplesampler.ShaderRegister = 0;
	simplesampler.RegisterSpace = 0;
	simplesampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_PARAMETER psimgrootparam = {};

	D3D12_DESCRIPTOR_RANGE descranges[2];
	{
		D3D12_DESCRIPTOR_RANGE& texsrvrange = descranges[0];
		texsrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		texsrvrange.NumDescriptors = 1;
		texsrvrange.BaseShaderRegister = 0;
		texsrvrange.RegisterSpace = 0;
		texsrvrange.OffsetInDescriptorsFromTableStart = 0;
	}
	{
		D3D12_DESCRIPTOR_RANGE& feedbackuavrange = descranges[1];
		feedbackuavrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		feedbackuavrange.NumDescriptors = 1;
		feedbackuavrange.BaseShaderRegister = 0;
		feedbackuavrange.RegisterSpace = 0;
		feedbackuavrange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

	psimgrootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	psimgrootparam.DescriptorTable.NumDescriptorRanges = 2;
	psimgrootparam.DescriptorTable.pDescriptorRanges = descranges;

	CD3DX12_ROOT_SIGNATURE_DESC emptyrootsignaturedesc = {};
	emptyrootsignaturedesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	emptyrootsignaturedesc.NumParameters = 1;
	emptyrootsignaturedesc.pParameters = &psimgrootparam;
	emptyrootsignaturedesc.NumStaticSamplers = 1;
	emptyrootsignaturedesc.pStaticSamplers = &simplesampler;
	DXASSERT(D3D12SerializeRootSignature(&emptyrootsignaturedesc, D3D_ROOT_SIGNATURE_VERSION_1, m_emptyrootsignatureblob.GetAddressOf(), m_rootsignatureerrors.GetAddressOf()))
		DXASSERT(m_creationdevice->CreateRootSignature(0, m_emptyrootsignatureblob.Get()->GetBufferPointer(), m_emptyrootsignatureblob->GetBufferSize(), IID_PPV_ARGS(m_emptyrootsignature.GetAddressOf())))
		basicpsodata.psodesc.graphicspsodesc.pRootSignature = m_emptyrootsignature.Get();

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


void DX12SamplerfeedbackApplication::Render()
{
	m_primarycmdlist.Reset();
	m_primarycmdlist->SetPipelineState(m_basicpso.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_emptyrootsignature.Get());
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
	{
		D3D12_VERTEX_BUFFER_VIEW vbview = m_planemodel.GetVBView();
		D3D12_INDEX_BUFFER_VIEW Ibview = m_planemodel.GetIBView();
		m_primarycmdlist->IASetVertexBuffers(0, 1, &vbview);
		m_primarycmdlist->IASetIndexBuffer(&Ibview);
	}


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
	m_primarycmdlist->DrawIndexedInstanced(m_planemodel.GetIndiciesCount(), 1, 0, 0, 0);
	backbufferbarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	backbufferbarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_primarycmdlist->ResourceBarrier(1, &backbufferbarrier);

	DXASSERT(m_primarycmdlist->Close())


		BasicRender();
}


