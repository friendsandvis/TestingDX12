#include"DX12ComputeApplication.h"

DX12ComputeApplication::DX12ComputeApplication()
	:m_planemodel(ModelDataUploadMode::COPY)
{
}

DX12ComputeApplication::~DX12ComputeApplication()
{
}

void DX12ComputeApplication::InitGfxPSO()
{
	PSOInitData psodata = {};
	psodata.type = GRAPHICS;

	//shaders to use
	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/BasicVertexShader_1.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/BasicPixelShader_1.hlsl", DX12Shader::ShaderType::PS);
	psodata.m_shaderstouse.push_back(vs);
	psodata.m_shaderstouse.push_back(ps);

	//build up graphic pso desc

	//shaderdesc
	psodata.psodesc.graphicspsodesc.VS.pShaderBytecode = vs->GetCompiledCode();
	psodata.psodesc.graphicspsodesc.VS.BytecodeLength = vs->GetCompiledCodeSize();
	psodata.psodesc.graphicspsodesc.PS.pShaderBytecode = ps->GetCompiledCode();
	psodata.psodesc.graphicspsodesc.PS.BytecodeLength = ps->GetCompiledCodeSize();

	//primitive setup
	psodata.psodesc.graphicspsodesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//rasterizer setup
	psodata.psodesc.graphicspsodesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psodata.psodesc.graphicspsodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psodata.psodesc.graphicspsodesc.RasterizerState.DepthClipEnable = FALSE;
	psodata.psodesc.graphicspsodesc.RasterizerState.AntialiasedLineEnable = FALSE;

	//rtv&sample setup
	psodata.psodesc.graphicspsodesc.SampleMask = UINT_MAX;
	psodata.psodesc.graphicspsodesc.SampleDesc.Count = 1;
	psodata.psodesc.graphicspsodesc.SampleDesc.Quality = 0;
	psodata.psodesc.graphicspsodesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	psodata.psodesc.graphicspsodesc.NumRenderTargets = 1;


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

	D3D12_DESCRIPTOR_RANGE texsrvrange = {};
	texsrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	texsrvrange.NumDescriptors = 1;
	texsrvrange.BaseShaderRegister = 0;
	texsrvrange.RegisterSpace = 0;
	texsrvrange.OffsetInDescriptorsFromTableStart = 0;

	psimgrootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	psimgrootparam.DescriptorTable.NumDescriptorRanges = 1;
	psimgrootparam.DescriptorTable.pDescriptorRanges = &texsrvrange;

	CD3DX12_ROOT_SIGNATURE_DESC emptyrootsignaturedesc = {};
	emptyrootsignaturedesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	emptyrootsignaturedesc.NumParameters = 1;
	emptyrootsignaturedesc.pParameters = &psimgrootparam;
	emptyrootsignaturedesc.NumStaticSamplers = 1;
	emptyrootsignaturedesc.pStaticSamplers = &simplesampler;
	DXASSERT(D3D12SerializeRootSignature(&emptyrootsignaturedesc, D3D_ROOT_SIGNATURE_VERSION_1, m_emptyrootsignatureblob.GetAddressOf(), m_rootsignatureerrors.GetAddressOf()))
		DXASSERT(m_creationdevice->CreateRootSignature(0, m_emptyrootsignatureblob.Get()->GetBufferPointer(), m_emptyrootsignatureblob->GetBufferSize(), IID_PPV_ARGS(m_emptyrootsignature.GetAddressOf())))
		psodata.psodesc.graphicspsodesc.pRootSignature = m_emptyrootsignature.Get();

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
		psodata.psodesc.graphicspsodesc.InputLayout.NumElements = 2;
		psodata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = simplevsinputelementdesc;

	}



	//blendstate setup
	//blend state has fixed rt count of 8
	for (size_t i = 0; i < 8; i++)
	{
		//same for all 8 rtvs
		psodata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].BlendEnable = FALSE;
		psodata.psodesc.graphicspsodesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}


	m_gfxpso.Init(m_creationdevice, psodata);
}

void DX12ComputeApplication::InitComputePSO()
{
	D3D12_DESCRIPTOR_RANGE computeuavrange = {};
	computeuavrange.NumDescriptors = 1;
	computeuavrange.OffsetInDescriptorsFromTableStart = 0;
	computeuavrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

	D3D12_ROOT_PARAMETER computerootparam = {};
	computerootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	computerootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	computerootparam.DescriptorTable.NumDescriptorRanges = 1;
	computerootparam.DescriptorTable.pDescriptorRanges = &computeuavrange;


	D3D12_ROOT_SIGNATURE_DESC computerootsigdesc = {};
	computerootsigdesc.Flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_NONE;
	computerootsigdesc.NumParameters = 1;
	computerootsigdesc.pParameters = &computerootparam;
	DXASSERT(D3D12SerializeRootSignature(&computerootsigdesc, D3D_ROOT_SIGNATURE_VERSION_1, m_computerootsignatureblob.GetAddressOf(), m_computerootsignatureerrors.GetAddressOf()))
	DXASSERT(m_creationdevice->CreateRootSignature(0,m_computerootsignatureblob->GetBufferPointer(),m_computerootsignatureblob->GetBufferSize(),IID_PPV_ARGS(m_computerootsignature.GetAddressOf())))

	PSOInitData computepsodata;

	computepsodata.type = PSOType::COMPUTE;
	DX12Shader* cs = new DX12Shader();
	cs->Init(L"shaders/TextureComputeShader.hlsl", DX12Shader::ShaderType::CS);
	computepsodata.psodesc.computepsodesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	computepsodata.psodesc.computepsodesc.CachedPSO = {};
	computepsodata.psodesc.computepsodesc.CS.BytecodeLength = cs->GetCompiledCodeSize();
	computepsodata.psodesc.computepsodesc.CS.pShaderBytecode = cs->GetCompiledCode();
	computepsodata.m_shaderstouse.push_back(cs);
	computepsodata.psodesc.computepsodesc.NodeMask = 0;
	computepsodata.psodesc.computepsodesc.pRootSignature=m_computerootsignature.Get();

		m_computepso.Init(m_creationdevice, computepsodata);
}

void DX12ComputeApplication::InitExtras()
{
	//init desc heaps
	D3D12_DESCRIPTOR_HEAP_DESC heapdesc = {};
	heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapdesc.NumDescriptors = 1;	//just creating for an srv
	heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	m_resaccessviewdescheap.Init(heapdesc, m_creationdevice);
	//compute one is identical to the non compute one
	m_resaccessviewdescheap_compute.Init(heapdesc, m_creationdevice);

	//init psos
	InitGfxPSO();
	InitComputePSO();

	//init assets
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	DXTexManager::LoadTexture(L"textures/tex3_miped.dds", m_redtexture.GetDXImageData());
	bool initsuccess = m_redtexture.Init(m_creationdevice);
	m_redtexture.SetName(L"GREENTEX");
	{
		


		
	}
	{
		//blank texture
		DX12ResourceCreationProperties rescreationprops;
		 DX12Resource::InitResourceCreationProperties(rescreationprops);
		rescreationprops.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		rescreationprops.resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		rescreationprops.resdesc.DepthOrArraySize=1;
		rescreationprops.resdesc.MipLevels = 1;
		rescreationprops.resdesc.Height=128;
		rescreationprops.resdesc.Width = 128;
		rescreationprops.resdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_SHADER_RESOURCE_VIEW_DESC customtexsrvdesc = {};
		customtexsrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		customtexsrvdesc.Texture2D.MipLevels = (UINT)rescreationprops.resdesc.MipLevels;
		customtexsrvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		m_customtexture.Init(m_creationdevice,rescreationprops , ResourceCreationMode::COMMITED);
		m_customtexture.SetName(L"CUSTOMTEX");
		m_customtexture.CreateSRV(m_creationdevice, customtexsrvdesc, m_resaccessviewdescheap.GetCPUHandleOffseted(0));
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc = {};
			uavdesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavdesc.Texture2D.MipSlice = 0;
			uavdesc.Texture2D.PlaneSlice = 0;
			m_customtexture.CreateUAV(m_creationdevice,uavdesc,m_resaccessviewdescheap_compute.GetCPUHandleOffseted(0) );
		}
	}

	//uploadassets
	m_uploadcommandlist.Reset();
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_redtexture.UploadTexture(m_uploadcommandlist);
	DXASSERT(m_uploadcommandlist->Close())
	
}

void DX12ComputeApplication::Render()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_swapchain.GetCurrentbackbufferIndex());
	m_primarycmdlist.Reset();
	{
		//first the compute task then gfx task
		m_primarycmdlist->SetPipelineState(m_computepso.GetPSO());
		m_primarycmdlist->SetComputeRootSignature(m_computerootsignature.Get());
		ID3D12DescriptorHeap* descheapstoset[1];
		descheapstoset[0] = m_resaccessviewdescheap_compute.GetDescHeap();
		m_primarycmdlist->SetDescriptorHeaps(1, descheapstoset);
		m_primarycmdlist->SetComputeRootDescriptorTable(0, m_resaccessviewdescheap_compute.GetGPUHandleOffseted(0));

		//transition custom tex to uav for compute access
		D3D12_RESOURCE_BARRIER customtexbarrier=m_customtexture.TransitionResState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		m_primarycmdlist->ResourceBarrier(1, &customtexbarrier);

		m_primarycmdlist->Dispatch(128, 128, 1);
		customtexbarrier = m_customtexture.TransitionResState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_primarycmdlist->ResourceBarrier(1, &customtexbarrier);

		

	}
	m_primarycmdlist->SetPipelineState(m_gfxpso.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_emptyrootsignature.Get());
	{
		ID3D12DescriptorHeap* descheapstoset[1];
		descheapstoset[0] = m_resaccessviewdescheap.GetDescHeap();
		m_primarycmdlist->SetDescriptorHeaps(1, descheapstoset);
	}
	m_primarycmdlist->SetGraphicsRootDescriptorTable(0, m_resaccessviewdescheap.GetGPUHandlefromstart());

	{
		m_primarycmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		{
			D3D12_VERTEX_BUFFER_VIEW vbview = m_planemodel.GetVBView();
			D3D12_INDEX_BUFFER_VIEW Ibview = m_planemodel.GetIBView();
			m_primarycmdlist->IASetVertexBuffers(0, 1, &vbview);
			m_primarycmdlist->IASetIndexBuffer(&Ibview);
		}
		{
			D3D12_VIEWPORT aviewport = GetViewport();

			D3D12_RECT ascissorrect = GetScissorRect();
			ascissorrect.right = aviewport.Width;
			ascissorrect.bottom = aviewport.Height;

			m_primarycmdlist->RSSetViewports(1, &aviewport);
			m_primarycmdlist->RSSetScissorRects(1, &ascissorrect);
		}
		
	}
	D3D12_RESOURCE_BARRIER backbufferbarrier = {};
	backbufferbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	backbufferbarrier.Transition.pResource = m_swapchain.GetBackBuffer(m_swapchain.GetCurrentbackbufferIndex());
	backbufferbarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	backbufferbarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	backbufferbarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_primarycmdlist->ResourceBarrier(1, &backbufferbarrier);
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE, nullptr);
	
	{
		float clearcolour[4] = {1.0f,1.0f,1.0f,1.0f};
		m_primarycmdlist->ClearRenderTargetView(rtvhandle, clearcolour,0,nullptr);
		m_primarycmdlist->DrawIndexedInstanced(m_planemodel.GetIndiciesCount(), 1, 0, 0, 0);
		
	}

	backbufferbarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	backbufferbarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_primarycmdlist->ResourceBarrier(1, &backbufferbarrier);
	DXASSERT(m_primarycmdlist->Close())

	BasicRender();
}