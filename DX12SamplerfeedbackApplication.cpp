#include"DX12SamplerfeedbackApplication.h"
#include"DXUtils.h"


DX12SamplerfeedbackApplication::DX12SamplerfeedbackApplication()
	:m_planemodel(ModelDataUploadMode::COPY),
	m_sfsupported(false)
{
}

DX12SamplerfeedbackApplication::~DX12SamplerfeedbackApplication()
{
}

void DX12SamplerfeedbackApplication::InitExtras()
{
	//init sampler feedback reserved resorce
	{
		DXTexManager::LoadTexture(L"textures/tex3_miped.dds",m_imagedataforreservedrestex);
		DX12ResourceCreationProperties reservedrestexprops;
		DX12ReservedResource::InitResourceCreationProperties(reservedrestexprops);
		reservedrestexprops.resdesc.Width = m_imagedataforreservedrestex.m_imagemetadata.width;
		reservedrestexprops.resdesc.Height = m_imagedataforreservedrestex.m_imagemetadata.height;
		reservedrestexprops.resdesc.MipLevels = m_imagedataforreservedrestex.m_imagemetadata.mipLevels;
		reservedrestexprops.resdesc.Format = m_imagedataforreservedrestex.m_imagemetadata.format;
		reservedrestexprops.resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		m_sfsreservedresourcetex.Init(m_creationdevice, reservedrestexprops);
		m_sfsreservedresourcetex.SetName(L"Greentexreservedresourcesfstest");
	}
	//check sampler feedback support
	D3D12_FEATURE_DATA_D3D12_OPTIONS7 option7features = {};
	DXASSERT(m_creationdevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &option7features, sizeof(option7features)))
		m_sfsupported=(option7features.SamplerFeedbackTier != D3D12_SAMPLER_FEEDBACK_TIER_NOT_SUPPORTED);

	//init pso
	InitBasicPSO();

	//init desc heaps
	D3D12_DESCRIPTOR_HEAP_DESC heapdesc = {};
	heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapdesc.NumDescriptors = 3;	//just creating for 2 srv and  a uav
	heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	m_resaccessviewdescheap.Init(heapdesc, m_creationdevice);

	D3D12_DESCRIPTOR_HEAP_DESC heapdescsrc = {};
	heapdescsrc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapdescsrc.NumDescriptors = 1;	//just creating for a uav staging
	heapdescsrc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_resaccessviewdescheapsrc.Init(heapdescsrc, m_creationdevice);

	//init assets
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	DXTexManager::LoadTexture(L"textures/tex3_miped.dds", m_redtexture.GetDXImageData());
	bool initsuccess = m_redtexture.Init(m_creationdevice);
	m_redtexture.SetName(L"GREENTEX");

	{
		
		samplerFeedbackUnitInitData sfunitinitdata = {};
		sfunitinitdata.feedbackmipregion.Depth = 1;
		sfunitinitdata.feedbackmipregion.Width = 4;
		sfunitinitdata.feedbackmipregion.Height = 4;
		sfunitinitdata.isMinMipMap = true;
		sfunitinitdata.allowreservedresourceuavclear = true;
		sfunitinitdata.feedbacktexrestopairwith=&m_sfsreservedresourcetex;
		sfunitinitdata.feedbacktexuavhandle = m_resaccessviewdescheapsrc.GetCPUHandleOffseted(0);
		sfunitinitdata.minlodtexsrvhandle = m_resaccessviewdescheap.GetCPUHandleOffseted(2);

		ComPtr<ID3D12Device8> device8;
		DXASSERT(m_creationdevice.As(&device8))

			if (m_sfsupported)
			{
				m_redtexfeedbackunit.Init(device8, sfunitinitdata);
				m_creationdevice->CopyDescriptorsSimple(1, m_resaccessviewdescheap.GetCPUHandleOffseted(1), m_resaccessviewdescheapsrc.GetCPUHandleOffseted(0), D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
	}

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC redtexsrvdesc = {};
		redtexsrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		redtexsrvdesc.Texture2D.MipLevels = (UINT)m_redtexture.GetTotalMipCount();
		redtexsrvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;


		//m_redtexture.CreateSRV(m_creationdevice, redtexsrvdesc, m_resaccessviewdescheap.GetCPUHandleOffseted(0));
		m_sfsreservedresourcetex.CreateSRV(m_creationdevice, redtexsrvdesc, m_resaccessviewdescheap.GetCPUHandleOffseted(0));
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

	D3D12_DESCRIPTOR_RANGE descranges[3];
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
	{
		D3D12_DESCRIPTOR_RANGE& texsrvrange = descranges[2];
		texsrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		texsrvrange.NumDescriptors = 1;
		texsrvrange.BaseShaderRegister = 1;
		texsrvrange.RegisterSpace = 0;
		texsrvrange.OffsetInDescriptorsFromTableStart = 0;
	}

	psimgrootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	psimgrootparam.DescriptorTable.NumDescriptorRanges = 3;
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
	if(m_sfsupported)
	{
		//clear feedback map first
		D3D12_CPU_DESCRIPTOR_HANDLE feedbackcpuhandle = m_resaccessviewdescheapsrc.GetCPUHandleOffseted(0);
		D3D12_GPU_DESCRIPTOR_HANDLE feedbackgpuhandle = m_resaccessviewdescheap.GetGPUHandleOffseted(1);
		UINT clearcolour[4] = { 0xFF,0xFF,0xFF,0xFF };
		SamplerFeedbackTexture& feedbacktex=m_redtexfeedbackunit.GetFeedbackTexture();
		m_primarycmdlist->ClearUnorderedAccessViewUint(feedbackgpuhandle, feedbackcpuhandle,feedbacktex.GetResource().Get() ,clearcolour , 0, nullptr);
	}
	m_primarycmdlist->DrawIndexedInstanced(m_planemodel.GetIndiciesCount(), 1, 0, 0, 0);
	if (m_sfsupported)
	{
		//issue the feedbackunit readback(i.e. after the draw command that actually writes the feeedback)
		ComPtr<ID3D12GraphicsCommandList1> primerycommandlist1;
		DXASSERT(m_primarycmdlist.GetcmdListComPtr().As(&primerycommandlist1))
			m_redtexfeedbackunit.Readback(primerycommandlist1);
	}
	backbufferbarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	backbufferbarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_primarycmdlist->ResourceBarrier(1, &backbufferbarrier);
	
	

	

	DXASSERT(m_primarycmdlist->Close())

		BasicRender();
	
}


