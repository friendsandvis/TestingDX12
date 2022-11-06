#include"RayTracingTestApplication_Advanced.h"
#define OFFSETGBUFFERSRVTEXTURESINRTGLOBALHEAP 2
#define USESIMPLECLOSESTHITSHADER



RayTracingApplicationAdvanced::RayTracingApplicationAdvanced()
	:m_loadedmodel(ModelDataUploadMode::COPY),
	m_comloadedmodel(ModelDataUploadMode::COPY),
	m_raytracingsupported(true),
	m_rtmode(false),
	m_trianglemodel(ModelDataUploadMode::COPY),
	m_planemodel(ModelDataUploadMode::COPY)
{
	m_maincameracontroller.SetCameratoControl(&m_maincamera);
	
}
RayTracingApplicationAdvanced::~RayTracingApplicationAdvanced()
{

}
void RayTracingApplicationAdvanced::PreRenderUpdate()
{
	m_maincameracontroller.Update();
}

void RayTracingApplicationAdvanced::RenderRaster()
{
	m_primarycmdlist.Reset();
	//set rtv
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_swapchain.GetCurrentbackbufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	m_primarycmdlist->SetPipelineState(m_pso.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_pso.GetRootSignature());
	XMMATRIX mvp = m_maincamera.GetMVP();
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvp, 0);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandlestoset[1] =
	{
		rtvhandle
	};
	m_primarycmdlist->OMSetRenderTargets(1, rtvhandlestoset, FALSE, &dsvhandle);
	float clearvalue[4] = { 1.0f,1.0f,1.0f,1.0f };
	float blackclearvalue[4] = { 0.0f,0.0f,0.0f,1.0f };
	m_primarycmdlist->ClearRenderTargetView(m_gbufferrtvheaps.GetCPUHandleOffseted(0), blackclearvalue, 0, nullptr);
	m_primarycmdlist->ClearRenderTargetView(rtvhandle, clearvalue, 0, nullptr);
	m_primarycmdlist->ClearDepthStencilView(dsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	{
		m_primarycmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D12_INDEX_BUFFER_VIEW ibview = m_loadedmodel.GetIBView();
		D3D12_VERTEX_BUFFER_VIEW vbview = m_loadedmodel.GetVBView();
		m_primarycmdlist->IASetVertexBuffers(0, 1, &vbview);
		m_primarycmdlist->IASetIndexBuffer(&ibview);

	}

	{
		D3D12_VIEWPORT aviewport = GetViewport();
		D3D12_VIEWPORT viewportstoset[1] =
		{aviewport};
		D3D12_RECT ascissorrect = GetScissorRect();
		D3D12_RECT scissorrectstoset[1] =
		{ ascissorrect };
		m_primarycmdlist->RSSetViewports(1, viewportstoset);
		m_primarycmdlist->RSSetScissorRects(1,scissorrectstoset);
	}
	m_primarycmdlist->DrawIndexedInstanced(m_loadedmodel.GetIndiciesCount(), 1, 0, 0, 0);
	DXASSERT(m_primarycmdlist->Close())
		BasicRender();
}
void RayTracingApplicationAdvanced::RenderGbuffer()
{
	m_gbufferrendercommandlist.Reset();
	m_gbufferrendercommandlist->SetPipelineState(m_gbufferpso.GetPSO());
	m_gbufferrendercommandlist->SetGraphicsRootSignature(m_pso.GetRootSignature());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandlestoset[3] =
	{
		m_gbufferrtvheaps.GetCPUHandleOffseted(0),
		m_gbufferrtvheaps.GetCPUHandleOffseted(1),
		m_gbufferrtvheaps.GetCPUHandleOffseted(2)
	};
	m_gbufferrendercommandlist->OMSetRenderTargets(3, rtvhandlestoset, FALSE, &dsvhandle);
	
	float blackclearvalue[4] = { 0.0f,0.0f,0.0f,0.0f };
	m_gbufferrendercommandlist->ClearRenderTargetView(m_gbufferrtvheaps.GetCPUHandleOffseted(0), blackclearvalue, 0, nullptr);
	m_gbufferrendercommandlist->ClearRenderTargetView(m_gbufferrtvheaps.GetCPUHandleOffseted(1), blackclearvalue, 0, nullptr);
	m_gbufferrendercommandlist->ClearRenderTargetView(m_gbufferrtvheaps.GetCPUHandleOffseted(2), blackclearvalue, 0, nullptr);
	m_gbufferrendercommandlist->ClearDepthStencilView(dsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	{
		D3D12_VIEWPORT aviewport = GetViewport();
		D3D12_VIEWPORT viewportstoset[3] =
		{ aviewport,aviewport,aviewport };
		D3D12_RECT ascissorrect = GetScissorRect();
		D3D12_RECT scissorrectstoset[3] =
		{ ascissorrect,ascissorrect,ascissorrect };
		m_gbufferrendercommandlist->RSSetViewports(3, viewportstoset);
		m_gbufferrendercommandlist->RSSetScissorRects(3, scissorrectstoset);
	}
	{
		m_gbufferrendercommandlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		
		XMMATRIX vp = m_maincamera.GetMVP();
		//m_loadedmodel.Draw(m_gbufferrendercommandlist, vp, 0);
		m_comloadedmodel.Draw(m_gbufferrendercommandlist, vp, 0);
		m_gbufferrendercommandlist.Close();
	}
	ID3D12CommandList* cmdliststoexecute[1] = { m_gbufferrendercommandlist.GetcmdList() };
	m_mainqueue.GetQueue()->ExecuteCommandLists(1, cmdliststoexecute);

}
void RayTracingApplicationAdvanced::RenderTextureOnScreenRT()
{
	m_primarycmdlist.Reset();
	//set rtv
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_swapchain.GetCurrentbackbufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	m_primarycmdlist->SetPipelineState(m_psortdisplay.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_psortdisplay.GetRootSignature());
	ID3D12DescriptorHeap* descheapstoset[1];
	descheapstoset[0] = m_rtdisplayresheap.GetDescHeap();
	m_primarycmdlist->SetDescriptorHeaps(1, descheapstoset);
	m_primarycmdlist->SetGraphicsRootDescriptorTable(0, m_rtdisplayresheap.GetGPUHandlefromstart());
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE, &dsvhandle);
	float clearvalue[4] = { 1.0f,1.0f,1.0f,1.0f };
	m_primarycmdlist->ClearRenderTargetView(rtvhandle, clearvalue, 0, nullptr);
	m_primarycmdlist->ClearDepthStencilView(dsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
		m_primarycmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D12_INDEX_BUFFER_VIEW ibview = m_planemodel.GetIBView();
		D3D12_VERTEX_BUFFER_VIEW vbview = m_planemodel.GetVBView();
		m_primarycmdlist->IASetVertexBuffers(0, 1, &vbview);
		m_primarycmdlist->IASetIndexBuffer(&ibview);
		{
			D3D12_VIEWPORT aviewport = GetViewport();
			D3D12_VIEWPORT viewportstoset[3] =
			{ aviewport,aviewport,aviewport };
			D3D12_RECT ascissorrect = GetScissorRect();
			D3D12_RECT scissorrectstoset[3] =
			{ ascissorrect,ascissorrect,ascissorrect };
			m_primarycmdlist->RSSetViewports(3, viewportstoset);
			m_primarycmdlist->RSSetScissorRects(3, scissorrectstoset);
		}
		m_primarycmdlist->DrawIndexedInstanced(m_planemodel.GetIndiciesCount(), 1, 0, 0, 0);
		DXASSERT(m_primarycmdlist->Close())
		BasicRender();
}
void RayTracingApplicationAdvanced::RenderTextureOnScreenGBuffer()
{
	m_primarycmdlist.Reset();
	//set rtv
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_swapchain.GetCurrentbackbufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	m_primarycmdlist->SetPipelineState(m_psogbufferdisplay.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_psogbufferdisplay.GetRootSignature());
	ID3D12DescriptorHeap* descheapstoset[1];
	descheapstoset[0] = m_gbuffersrvheap.GetDescHeap();
	m_primarycmdlist->SetDescriptorHeaps(1, descheapstoset);
	m_primarycmdlist->SetGraphicsRootDescriptorTable(0, m_gbuffersrvheap.GetGPUHandlefromstart());
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE, &dsvhandle);
	float clearvalue[4] = { 1.0f,1.0f,1.0f,1.0f };
	m_primarycmdlist->ClearRenderTargetView(rtvhandle, clearvalue, 0, nullptr);
	m_primarycmdlist->ClearDepthStencilView(dsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	m_primarycmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D12_INDEX_BUFFER_VIEW ibview = m_planemodel.GetIBView();
	D3D12_VERTEX_BUFFER_VIEW vbview = m_planemodel.GetVBView();
	m_primarycmdlist->IASetVertexBuffers(0, 1, &vbview);
	m_primarycmdlist->IASetIndexBuffer(&ibview);
	{
		D3D12_VIEWPORT aviewport = GetViewport();
		D3D12_VIEWPORT viewportstoset[3] =
		{ aviewport,aviewport,aviewport };
		D3D12_RECT ascissorrect = GetScissorRect();
		D3D12_RECT scissorrectstoset[3] =
		{ ascissorrect,ascissorrect,ascissorrect };
		m_primarycmdlist->RSSetViewports(3, viewportstoset);
		m_primarycmdlist->RSSetScissorRects(3, scissorrectstoset);
	}
	m_primarycmdlist->DrawIndexedInstanced(m_planemodel.GetIndiciesCount(), 1, 0, 0, 0);
	DXASSERT(m_primarycmdlist->Close())
		BasicRender();
}


void RayTracingApplicationAdvanced::RenderRT()
{
	
	
	//rt pass
	{
		D3D12_DISPATCH_RAYS_DESC dispatchraysdesc = {};
		dispatchraysdesc.Width = m_swapchain.GetSwapchainWidth();
		dispatchraysdesc.Height = m_swapchain.GetSwapchainHeight();
		dispatchraysdesc.Depth = 1;
		dispatchraysdesc.RayGenerationShaderRecord.StartAddress = m_rgsrecords.GetResource()->GetGPUVirtualAddress();;
		dispatchraysdesc.RayGenerationShaderRecord.SizeInBytes = sizeof(BasicShaderRecord);
		dispatchraysdesc.MissShaderTable.StartAddress = m_missrecords.GetResource()->GetGPUVirtualAddress();
		dispatchraysdesc.MissShaderTable.SizeInBytes = m_missrecords.GetSize();
		dispatchraysdesc.MissShaderTable.StrideInBytes= sizeof(BasicShaderRecord);
		dispatchraysdesc.HitGroupTable.StartAddress = m_hitrecords.GetResource()->GetGPUVirtualAddress();
		dispatchraysdesc.HitGroupTable.SizeInBytes =m_hitrecords.GetSize();
		dispatchraysdesc.HitGroupTable.StrideInBytes = sizeof(BasicShaderRecord);


		m_rtcommandlist.Reset();
		if (m_rgsrecords.GetCurrentResourceState()!= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		{
			D3D12_RESOURCE_BARRIER rgsrecordbuffertransitionbarrier = m_rgsrecords.TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		m_rtcommandlist->ResourceBarrier(1, &rgsrecordbuffertransitionbarrier);
	}
		ID3D12DescriptorHeap* heapstoset[1] = { m_rtresheap_global.GetDescHeap()};
		m_rtcommandlist->SetDescriptorHeaps(1, heapstoset);
		ID3D12RootSignature* rtglobalrootsig = m_simplertpso.GetGlobalRootSignature();
		assert(rtglobalrootsig != nullptr);
		m_rtcommandlist->SetComputeRootSignature(rtglobalrootsig);
		m_rtcommandlist->SetComputeRootDescriptorTable(0, m_rtresheap_global.GetGPUHandleOffseted(0));
		XMMATRIX orthoproj = XMMatrixOrthographicLH(2.0F, 2.0F, -100.0f, 100.0f);
		
		XMMATRIX invproj= XMMatrixInverse(nullptr, m_maincamera.GetProjection());
		XMMATRIX invview = XMMatrixInverse(nullptr, m_maincamera.GetView());
		MatrixPair invmatpair;
		invmatpair.m1 = invproj;
		invmatpair.m2 = invview;
		XMMATRIX vp = m_maincamera.GetVP();
		m_rtcommandlist->SetComputeRoot32BitConstants(1, (sizeof(MatrixPair) / 4),&invmatpair, 0);
		m_rtcommandlist->SetComputeRoot32BitConstants(2, (sizeof(XMMATRIX) / 4), &vp, 0);
		m_rtcommandlist->SetPipelineState1(m_simplertpso.GetPipelineStateObject());
		float  clearcolour[4] = {1.0f,1.0f,1.0f,1.0f};
		m_rtcommandlist->ClearUnorderedAccessViewFloat(m_rtresheap_global.GetGPUHandlefromstart(),m_rtresheap_globalupload.GetCPUHandlefromstart(),m_rtouput.GetResource().Get(),clearcolour,0,nullptr);
		m_rtcommandlist->DispatchRays(&dispatchraysdesc);
		m_rtcommandlist.Close();
	}

	
	
	//execute rt cmd list first
	{
		ID3D12CommandList* cmdliststoexecute[1] = {m_rtcommandlist.GetcmdList()};
		m_mainqueue.GetQueue()->ExecuteCommandLists(1, cmdliststoexecute);
	}
		
}
void RayTracingApplicationAdvanced::Render()
{
	RenderGbuffer();
	//make sure both the modes are drawing same geometry to view  real-time tooggle diffrence.
	if (m_rtmode)
	{
		
		 RenderRT();
		 RenderTextureOnScreenRT();
	}
	else
	{
		
		
		RenderTextureOnScreenGBuffer();
	}
}

void RayTracingApplicationAdvanced::InitExtras()
{
	
	//check ray tracing  support
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 option5features = {};
	DXASSERT(m_creationdevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &option5features, sizeof(option5features)))
		m_raytracingsupported = (option5features.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED);
	BasicModelManager::LoadModel(m_creationdevice, "models/cube.dae", m_loadedmodel, VERTEXVERSION2);
	BasicModelManager::LoadModel(m_creationdevice, "models/cubes2.dae", m_comloadedmodel, VERTEXVERSION2);
	BasicModelManager::InitTriangleModel(m_creationdevice, m_trianglemodel);
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	float aspectratio = m_swapchain.GetSwapchainWidth() / (float)m_swapchain.GetSwapchainHeight();
	
	//shader records buffer init
	{
		DX12ResourceCreationProperties rgsrecordsprops = {};
		DX12Buffer::InitResourceCreationProperties(rgsrecordsprops);
		//rgs records will contain only 1 record for now.
		rgsrecordsprops.resdesc.Width = sizeof(BasicShaderRecord) * 1;
		rgsrecordsprops.resheapprop.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		m_rgsrecords.Init(m_creationdevice, rgsrecordsprops, ResourceCreationMode::COMMITED);
		DX12ResourceCreationProperties missshaderrecordsprops = rgsrecordsprops;
		m_missrecords.Init(m_creationdevice, missshaderrecordsprops, ResourceCreationMode::COMMITED);
		DX12ResourceCreationProperties hitshaderrecordsprops = rgsrecordsprops;
		m_hitrecords.Init(m_creationdevice, hitshaderrecordsprops, ResourceCreationMode::COMMITED);
		
		
	}

	//init gbuffer textures
	//gbuffer rtv heap and gbuffer srv heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC gbufferrtvheapdesc = {};
		gbufferrtvheapdesc.NumDescriptors = 3;
		gbufferrtvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		m_gbufferrtvheaps.Init(gbufferrtvheapdesc, m_creationdevice);
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC gbuffersrvheapdesc = {};
		gbuffersrvheapdesc.NumDescriptors = NUMGBUFFERTEXTURES;
		gbuffersrvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		gbuffersrvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_gbuffersrvheap.Init(gbuffersrvheapdesc, m_creationdevice);
	}
	//heap to hold resources used by rt(global)
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtglobalheapdesc = {};
		rtglobalheapdesc.NumDescriptors = 2+NUMGBUFFERTEXTURES;//just 1 uav for now and 1 srv and srvs for gbuffer textures
		rtglobalheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		rtglobalheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_rtresheap_global.Init(rtglobalheapdesc,m_creationdevice);
		D3D12_DESCRIPTOR_HEAP_DESC rtglobalheapdescupload = {};
		rtglobalheapdescupload.NumDescriptors = 1;//just 1 uav for now
		rtglobalheapdescupload.Type= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		rtglobalheapdescupload.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_rtresheap_globalupload.Init(rtglobalheapdescupload, m_creationdevice);
	}
	//heap to hold resources used by rt output display pass
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapdesc = {};
		heapdesc.NumDescriptors = 1;//just 1 srv for now.
		heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_rtdisplayresheap.Init(heapdesc, m_creationdevice);
	}
	DX12ResourceCreationProperties gbuffertextureprops;
	DX12TextureSimple::InitResourceCreationProperties(gbuffertextureprops);
	gbuffertextureprops.resdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	gbuffertextureprops.resdesc.Width = m_swapchain.GetSwapchainWidth();
	gbuffertextureprops.resdesc.Height = m_swapchain.GetSwapchainHeight();
	gbuffertextureprops.useclearvalue = true;
	gbuffertextureprops.resdesc.Flags |= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	gbuffertextureprops.resdesc.Flags |= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	gbuffertextureprops.optimizedclearvalue.Format = gbuffertextureprops.resdesc.Format;
	gbuffertextureprops.resinitialstate = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
	
	gbuffertextureprops.optimizedclearvalue.Color[0] = 0.0f;
	gbuffertextureprops.optimizedclearvalue.Color[1] = 0.0f;
	gbuffertextureprops.optimizedclearvalue.Color[2] = 0.0f;
	gbuffertextureprops.optimizedclearvalue.Color[3] = 0.0f;
	m_gbuffernormal.Init(m_creationdevice, gbuffertextureprops, ResourceCreationMode::COMMITED);
	m_gbuffernormal.SetName(L"GBUFFER_NORMAL");
	m_gbufferposition.Init(m_creationdevice, gbuffertextureprops, ResourceCreationMode::COMMITED);
	m_gbufferposition.SetName(L"GBUFFER_POSITION");
	m_gbufferalbedo.Init(m_creationdevice, gbuffertextureprops, ResourceCreationMode::COMMITED);
	m_gbufferalbedo.SetName(L"GBUFFER_ALBEDO");
	{
		DX12ResourceCreationProperties rtoutputtexprops = {};
		DX12TextureSimple::InitResourceCreationProperties(rtoutputtexprops);
		
		rtoutputtexprops.resdesc.Height = m_swapchain.GetSwapchainHeight();
		rtoutputtexprops.resdesc.Width = m_swapchain.GetSwapchainWidth();
		rtoutputtexprops.resdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		rtoutputtexprops.useclearvalue = false;
		rtoutputtexprops.resdesc.Flags=D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		
		m_rtouput.Init(m_creationdevice,rtoutputtexprops,ResourceCreationMode::COMMITED );
		m_rtouput.SetName(L"RToutput");
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc = {};
		uavdesc.Texture2D.MipSlice = 0;
		uavdesc.Texture2D.PlaneSlice = 0;
		uavdesc.ViewDimension = D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2D;
		m_rtouput.CreateUAV(m_creationdevice, uavdesc, m_rtresheap_global.GetCPUHandleOffseted(0));
		m_rtouput.CreateUAV(m_creationdevice, uavdesc, m_rtresheap_globalupload.GetCPUHandleOffseted(0));
		{
			//create tlas srv
			//empty tlas
			D3D12_SHADER_RESOURCE_VIEW_DESC tlassrrvdesc = {};
			tlassrrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			tlassrrvdesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
			//m_creationdevice->CreateShaderResourceView(nullptr, &tlassrrvdesc, m_rtresheap_global.GetCPUHandleOffseted(1));
			
		}
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
			srvdesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
			srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvdesc.Texture2D.MipLevels = 1;
			srvdesc.Texture2D.MostDetailedMip = 0;
			srvdesc.Texture2D.ResourceMinLODClamp = 0.0f;

			m_rtouput.CreateSRV(m_creationdevice, srvdesc, m_rtdisplayresheap.GetCPUHandleOffseted(0));
		}
	}
	D3D12_RENDER_TARGET_VIEW_DESC gbufferrtvdesc = {};
	gbufferrtvdesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	gbufferrtvdesc.Texture2D.MipSlice = 0;
	gbufferrtvdesc.Texture2D.PlaneSlice = 0;
	D3D12_SHADER_RESOURCE_VIEW_DESC gbuffersrvdesc = {};
	gbuffersrvdesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	gbuffersrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	gbuffersrvdesc.Texture2D.MipLevels = 1;
	gbuffersrvdesc.Texture2D.MostDetailedMip = 0;
	gbuffersrvdesc.Texture2D.ResourceMinLODClamp = 0.0f;
	D3D12_UNORDERED_ACCESS_VIEW_DESC gbufferuavdesc = {};
	gbufferuavdesc.Texture2D.MipSlice = 0;
	gbufferuavdesc.Texture2D.PlaneSlice = 0;
	gbufferuavdesc.ViewDimension = D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2D;
	

	//create rtv for gbuffer
	m_gbufferalbedo.CreateRTV(m_creationdevice, gbufferrtvdesc, m_gbufferrtvheaps.GetCPUHandleOffseted(0));
	m_gbuffernormal.CreateRTV(m_creationdevice, gbufferrtvdesc, m_gbufferrtvheaps.GetCPUHandleOffseted(1));
	m_gbufferposition.CreateRTV(m_creationdevice, gbufferrtvdesc, m_gbufferrtvheaps.GetCPUHandleOffseted(2));
	//create srv for gbuffer
	
	m_gbufferalbedo.CreateSRV(m_creationdevice, gbuffersrvdesc, m_gbuffersrvheap.GetCPUHandleOffseted(0));
	m_gbuffernormal.CreateSRV(m_creationdevice, gbuffersrvdesc, m_gbuffersrvheap.GetCPUHandleOffseted(1));
	m_gbufferposition.CreateSRV(m_creationdevice, gbuffersrvdesc, m_gbuffersrvheap.GetCPUHandleOffseted(2));
	//create uav for gbuffer
	m_gbufferalbedo.CreateUAV(m_creationdevice, gbufferuavdesc, m_rtresheap_global.GetCPUHandleOffseted(OFFSETGBUFFERSRVTEXTURESINRTGLOBALHEAP+0));
	m_gbuffernormal.CreateUAV(m_creationdevice, gbufferuavdesc, m_rtresheap_global.GetCPUHandleOffseted(OFFSETGBUFFERSRVTEXTURESINRTGLOBALHEAP + 1));
	m_gbufferposition.CreateUAV(m_creationdevice, gbufferuavdesc, m_rtresheap_global.GetCPUHandleOffseted(OFFSETGBUFFERSRVTEXTURESINRTGLOBALHEAP + 2));
	

	InitGbufferPSO();
	InitPSO_RTRaster();
	InitRTDisplayPSO();
	InitGBufferDisplayPSO();
	
	
	m_loadedmodel.UploadModelDatatoBuffers();
	m_comloadedmodel.UploadModelDatatoBuffers();
	//ray tracing inits
	if(m_raytracingsupported)
	{
		m_rtcommandlist.Init(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, m_creationdevice);
		m_rtcommandlist.SetName(L"RTCommandlist");
		DXASSERT(m_creationdevice.As(&m_device5))
		loadedmodelasblas.Init(m_creationdevice,m_loadedmodel);
		m_comploadedmodelblas.Init(m_creationdevice, m_comloadedmodel);
		loadedmodelasblas.Build(m_device5);
		m_comploadedmodelblas.Build(m_device5);
		InitRTPSO();

		{
			const std::vector<ModelAccelerationStructureBLAS*>& compmodelblas = m_comploadedmodelblas.GetBlas();
			vector< D3D12_RAYTRACING_INSTANCE_DESC> instancedescs;
			for (ModelAccelerationStructureBLAS* modelblas : compmodelblas)
			{
				D3D12_RAYTRACING_INSTANCE_DESC aninstancedesc = {};
				aninstancedesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAGS::D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
				aninstancedesc.AccelerationStructure = modelblas->GetBLAS().GetResource()->GetGPUVirtualAddress();
				aninstancedesc.InstanceID = 0;
				aninstancedesc.InstanceMask = 1;
				aninstancedesc.InstanceContributionToHitGroupIndex = 0;
				RaytracingCommon::InitAsIdentityMatrix(aninstancedesc.Transform);
				
				instancedescs.push_back(aninstancedesc);
			}
			
			loadedmodelastlas.Init(m_creationdevice, instancedescs);
			
			loadedmodelastlas.Build(m_device5);
			loadedmodelastlas.CreateSRV(m_creationdevice, m_rtresheap_global.GetCPUHandleOffseted(1));
		}
		
	}
	
	{
		m_gbufferrendercommandlist.Init(D3D12_COMMAND_LIST_TYPE_DIRECT, m_creationdevice);
		m_gbufferrendercommandlist.SetName(L"Gbuffer render commandlist");
	}

	m_uploadcommandlist.Reset();
	m_trianglemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_loadedmodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_comloadedmodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	if (m_raytracingsupported)
	{
		ComPtr<ID3D12GraphicsCommandList4> cmdlist4;
		DXASSERT(m_uploadcommandlist.GetcmdListComPtr().As(&cmdlist4))

		loadedmodelasblas.IssueBuild(cmdlist4);
		m_comploadedmodelblas.IssueBuild(cmdlist4);
		loadedmodelastlas.IssueBuild(cmdlist4);
	}
	DXASSERT(m_uploadcommandlist->Close());
}

void RayTracingApplicationAdvanced::InitGbufferPSO()
{
	PSOInitData psoinitdata;
	psoinitdata.type = PSOType::GRAPHICS;

	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/raytracing/GeneralRenderVertexShader.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/raytracing/GeneralRenderTestPixelShader.hlsl", DX12Shader::ShaderType::PS);
	psoinitdata.m_shaderstouse.push_back(vs); psoinitdata.m_shaderstouse.push_back(ps);
	DX12PSO::DefaultInitPSOData(psoinitdata);
	psoinitdata.psodesc.graphicspsodesc.PS.BytecodeLength = ps->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.PS.pShaderBytecode = ps->GetCompiledCode();
	psoinitdata.psodesc.graphicspsodesc.VS.BytecodeLength = vs->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.VS.pShaderBytecode = vs->GetCompiledCode();
	//rtv setup
	psoinitdata.psodesc.graphicspsodesc.NumRenderTargets = 3;
	psoinitdata.psodesc.graphicspsodesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	psoinitdata.psodesc.graphicspsodesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	psoinitdata.psodesc.graphicspsodesc.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;

	//root signature setup
	{
		{
			std::vector< D3D12_ROOT_PARAMETER>rootparams;
			std::vector<D3D12_STATIC_SAMPLER_DESC> staticsamplers;
			
			D3D12_ROOT_PARAMETER arootparam = {};
			arootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			arootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			arootparam.Constants.Num32BitValues = sizeof(XMMATRIX) / 4;
			arootparam.Constants.RegisterSpace = 0;
			arootparam.Constants.ShaderRegister = 0;
			rootparams.push_back(arootparam);
			
			psoinitdata.rootsignature.BuidDesc(rootparams, staticsamplers);


		}

		//input assembler setup
		D3D12_INPUT_ELEMENT_DESC inputelements[2] = { 0 };
		inputelements[0].SemanticName = "POS";
		inputelements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputelements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelements[0].InputSlot = 0;
		inputelements[1].SemanticName = "NORMAL";
		inputelements[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputelements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelements[1].InputSlot = 0;
		inputelements[1].AlignedByteOffset = sizeof(float) * 3;//after three floats is normal

		psoinitdata.psodesc.graphicspsodesc.InputLayout.NumElements = 2;
		psoinitdata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = inputelements;

		
		
	}
	m_gbufferpso.Init(m_creationdevice, psoinitdata);
}
void RayTracingApplicationAdvanced::InitRTDisplayPSO()
{
	PSOInitData basicpsodata = {};
	basicpsodata.type = GRAPHICS;

	//shaders to use
	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/RTextras/VS_displayTexOnScreen.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/RTextras/PS_displayTexOnScreen.hlsl", DX12Shader::ShaderType::PS);
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
	//root signature
	{
		vector<D3D12_ROOT_PARAMETER> rootparams;
		
		vector<D3D12_STATIC_SAMPLER_DESC> staticsamplers;
		D3D12_STATIC_SAMPLER_DESC asampler = StaticSamplerManager::GetDefaultStaticSamplerDesc();
		asampler.Filter = D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticsamplers.push_back(asampler);
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
		rootparams.push_back(psimgrootparam);




		basicpsodata.rootsignature.BuidDesc(rootparams,staticsamplers,D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	}

	m_psortdisplay.Init(m_creationdevice, basicpsodata);
}
void RayTracingApplicationAdvanced::InitGBufferDisplayPSO()
{
	PSOInitData basicpsodata = {};
	basicpsodata.type = GRAPHICS;

	//shaders to use
	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/RTextras/VS_displayTexOnScreen.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/RTextras/PS_GBufferDisplay.hlsl", DX12Shader::ShaderType::PS);
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
	//root signature
	{
		vector<D3D12_ROOT_PARAMETER> rootparams;

		vector<D3D12_STATIC_SAMPLER_DESC> staticsamplers;
		D3D12_STATIC_SAMPLER_DESC asampler = StaticSamplerManager::GetDefaultStaticSamplerDesc();
		asampler.Filter = D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticsamplers.push_back(asampler);
		D3D12_ROOT_PARAMETER psimgrootparam = {};

		D3D12_DESCRIPTOR_RANGE texsrvrange = {};
		texsrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		texsrvrange.NumDescriptors = NUMGBUFFERTEXTURES;
		texsrvrange.BaseShaderRegister = 0;
		texsrvrange.RegisterSpace = 0;
		texsrvrange.OffsetInDescriptorsFromTableStart = 0;

		psimgrootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		psimgrootparam.DescriptorTable.NumDescriptorRanges = 1;
		psimgrootparam.DescriptorTable.pDescriptorRanges = &texsrvrange;
		rootparams.push_back(psimgrootparam);




		basicpsodata.rootsignature.BuidDesc(rootparams, staticsamplers, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	}

	m_psogbufferdisplay.Init(m_creationdevice, basicpsodata);
}
void RayTracingApplicationAdvanced::InitRTPSO()
{
	D3D12_RAYTRACING_SHADER_CONFIG simplertshaderconfig = {};
	simplertshaderconfig.MaxPayloadSizeInBytes = sizeof(XMMATRIX);
	simplertshaderconfig.MaxAttributeSizeInBytes = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;
	m_simplertpso.AddShaderConfig(simplertshaderconfig, "simpleshaderconfig");
	DX12Shader* rgs = new DX12Shader();
	rgs->Init(L"shaders/raytracing/RT/simplergs3_Advance.hlsl", DX12Shader::ShaderType::RT);
	m_simplertpso.AddShader(rgs, L"rgsmain", L"SimpleRGS",RTPSOSHADERTYPE::RAYGEN);
	DX12Shader* simplemiss = new DX12Shader();
	simplemiss->Init(L"shaders/raytracing/RT/simplemiss.hlsl", DX12Shader::ShaderType::RT);
	m_simplertpso.AddShader(simplemiss, L"missmain", L"SimpleMISS", RTPSOSHADERTYPE::MISS);
	
		DX12Shader* simplech = new DX12Shader();
#ifdef USESIMPLECLOSESTHITSHADER
		simplech->Init(L"shaders/raytracing/RT/simpleclosesthit.hlsl", DX12Shader::ShaderType::RT);
#else
		simplech->Init(L"shaders/raytracing/RT/closesthit_fetchgbuffer.hlsl", DX12Shader::ShaderType::RT);
#endif // USESIMPLECLOSESTHITSHADER

		
		m_simplertpso.AddShader(simplech, L"closesthitmain", L"SimpleCH", RTPSOSHADERTYPE::CLOSESTHIT);
		D3D12_HIT_GROUP_DESC simplehitgroupdesc = {};
		simplehitgroupdesc.Type = D3D12_HIT_GROUP_TYPE::D3D12_HIT_GROUP_TYPE_TRIANGLES;
		simplehitgroupdesc.HitGroupExport = L"SimpleHIT";
		simplehitgroupdesc.ClosestHitShaderImport = L"SimpleCH";

		m_simplertpso.AddHitGroup(simplehitgroupdesc);
	
	m_simplertpso.SetPipelineConfig();
	{
		DX12RootSignature rtglobalrootsig;
		vector<D3D12_ROOT_PARAMETER> rootparams;
		D3D12_ROOT_PARAMETER param1 = {};
		param1.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		param1.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		D3D12_DESCRIPTOR_RANGE descranges[3];
		descranges[0].NumDescriptors = 1;
		descranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descranges[0].NumDescriptors = 1;
		descranges[0].BaseShaderRegister = 0;
		descranges[0].RegisterSpace = 0;
		descranges[0].OffsetInDescriptorsFromTableStart = 0;
		descranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descranges[1].NumDescriptors = 1;
		descranges[1].BaseShaderRegister = 0;
		descranges[1].RegisterSpace = 0;
		descranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		descranges[2].RangeType= D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		descranges[2].NumDescriptors = NUMGBUFFERTEXTURES;
		descranges[2].BaseShaderRegister = 1;
		descranges[2].RegisterSpace = 0;
		descranges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		
		D3D12_ROOT_PARAMETER param2 = {};
		param2.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		param2.Constants.Num32BitValues = sizeof(MatrixPair) / 4;
		param2.Constants.ShaderRegister = 0;
		param2.Constants.RegisterSpace = 0;
		param2.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		D3D12_ROOT_PARAMETER param3 = {};
		param3.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		param3.Constants.Num32BitValues = sizeof(XMMATRIX) / 4;
		param3.Constants.ShaderRegister = 1;
		param3.Constants.RegisterSpace = 0;
		param3.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		

		
		param1.DescriptorTable.pDescriptorRanges = descranges;
		param1.DescriptorTable.NumDescriptorRanges = 3;
		rootparams.push_back(param1);
		rootparams.push_back(param2);
		rootparams.push_back(param3);
		vector<D3D12_STATIC_SAMPLER_DESC> staticsamplersused;
		
		
		

		

		rtglobalrootsig.BuidDesc(rootparams, staticsamplersused, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_NONE);
		rtglobalrootsig.Init(m_creationdevice);
		m_simplertpso.SetGlobalRootSignature(rtglobalrootsig);
	}
	m_simplertpso.Init(m_device5);
	//upload actual data to shader recods
	{
		
		//pass rgs records to buffer
		{
			//rgs
			BasicShaderRecord record = {};
			void* shaderidentifier = m_simplertpso.GetIdentifier(L"SimpleRGS");
			assert(shaderidentifier != nullptr);
			record.SetShaderidentifier(shaderidentifier);
			BufferMapParams rgsmapparams = {};
			rgsmapparams.range.Begin = 0;
			rgsmapparams.range.End = 0;
			void* mappedbuffer = m_rgsrecords.Map(rgsmapparams);
			assert(mappedbuffer != nullptr);
			//copy over rgs records
			memcpy(mappedbuffer, &record, sizeof(BasicShaderRecord));
			rgsmapparams.range.End = m_rgsrecords.GetSize();
			m_rgsrecords.UnMap(rgsmapparams);
		}
		//pass missshader records to buffer
		{
			//rgs
			BasicShaderRecord record = {};
			void* shaderidentifier = m_simplertpso.GetIdentifier(L"SimpleMISS");
			assert(shaderidentifier != nullptr);
			record.SetShaderidentifier(shaderidentifier);
			BufferMapParams mapparams = {};
			mapparams.range.Begin = 0;
			mapparams.range.End = 0;
			void* mappedbuffer = m_missrecords.Map(mapparams);
			assert(mappedbuffer != nullptr);
			//copy over miss records
			memcpy(mappedbuffer, &record, sizeof(BasicShaderRecord));
			mapparams.range.End = m_missrecords.GetSize();
			m_missrecords.UnMap(mapparams);
		}
		//pass hit group records to buffer
		{
			
			BasicShaderRecord record = {};
			void* shaderidentifier = m_simplertpso.GetIdentifier(L"SimpleHIT",true);
			assert(shaderidentifier != nullptr);
			record.SetShaderidentifier(shaderidentifier);
			BufferMapParams mapparams = {};
			mapparams.range.Begin = 0;
			mapparams.range.End = 0;
			void* mappedbuffer = m_hitrecords.Map(mapparams);
			assert(mappedbuffer != nullptr);
			//copy over hit group records
			memcpy(mappedbuffer, &record, sizeof(BasicShaderRecord));
			mapparams.range.End = m_hitrecords.GetSize();
			m_hitrecords.UnMap(mapparams);
		}

	}
}
void RayTracingApplicationAdvanced::InitPSO_RTRaster()
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputelements;
	DXVertexManager::BuildDefaultInputelementdesc(inputelements, m_loadedmodel.GetVertexVersionUsed());
	PSOInitData psoinitdata;
	psoinitdata.type = PSOType::GRAPHICS;

	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/raytracing/RTRasterVertexShader.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/raytracing/RTRasterPixelShader.hlsl", DX12Shader::ShaderType::PS);
	psoinitdata.m_shaderstouse.push_back(vs); psoinitdata.m_shaderstouse.push_back(ps);
	DX12PSO::DefaultInitPSOData(psoinitdata);
	psoinitdata.psodesc.graphicspsodesc.PS.BytecodeLength = ps->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.PS.pShaderBytecode = ps->GetCompiledCode();
	psoinitdata.psodesc.graphicspsodesc.VS.BytecodeLength = vs->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.VS.pShaderBytecode = vs->GetCompiledCode();
	
	//root signature setup
	{
		{
			std::vector<D3D12_ROOT_PARAMETER> rootparams;
			std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
			
			D3D12_ROOT_PARAMETER arootparam;
			arootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			arootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			arootparam.Constants.Num32BitValues = sizeof(XMMATRIX) / 4;
			arootparam.Constants.RegisterSpace = 0;
			arootparam.Constants.ShaderRegister = 0;
			rootparams.push_back(arootparam);
			psoinitdata.rootsignature.BuidDesc(rootparams, samplers);
		}

		//input assembler setup

		psoinitdata.psodesc.graphicspsodesc.InputLayout.NumElements = (UINT)inputelements.size();
		psoinitdata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = inputelements.data();

		
		
	}


	m_pso.Init(m_creationdevice, psoinitdata);
}


void RayTracingApplicationAdvanced::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CHAR)
	{
		char charpressed = (char)wParam;
		if (charpressed == '1')
		{
			m_rtmode = !m_rtmode;
		}
	}
	m_maincameracontroller.ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);

}