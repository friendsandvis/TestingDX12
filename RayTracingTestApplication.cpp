#include"RayTracingTestApplication.h"




RayTracingApplication::RayTracingApplication()
	:m_planemodel(ModelDataUploadMode::COPY),
	m_cubemodel(ModelDataUploadMode::COPY),
	m_loadedmodel(ModelDataUploadMode::COPY)
{
	m_maincameracontroller.SetCameratoControl(&m_maincamera);
}
RayTracingApplication::~RayTracingApplication()
{

}
void RayTracingApplication::PreRenderUpdate()
{
	m_maincameracontroller.Update();
}

void RayTracingApplication::Render()
{
	m_primarycmdlist.Reset();
	//set rtv
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_swapchain.GetCurrentbackbufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	m_primarycmdlist->SetPipelineState(m_pso.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_rootsignature.GetRootSignature());
	XMMATRIX mvp = m_maincamera.GetMVP();
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvp, 0);
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE, &dsvhandle);
	float clearvalue[4] = { 1.0f,1.0f,1.0f,1.0f };
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

		D3D12_RECT ascissorrect = GetScissorRect();
		m_primarycmdlist->RSSetViewports(1, &aviewport);
		m_primarycmdlist->RSSetScissorRects(1, &ascissorrect);
	}
	m_primarycmdlist->DrawIndexedInstanced(m_loadedmodel.GetIndiciesCount(), 1, 0, 0, 0);
	DXASSERT(m_primarycmdlist->Close())
		BasicRender();
}

void RayTracingApplication::InitExtras()
{
	BasicModelManager::LoadModel(m_creationdevice, "models/cube.dae", m_loadedmodel, VERTEXVERSION2);
	float aspectratio = m_swapchain.GetSwapchainWidth() / (float)m_swapchain.GetSwapchainHeight();

	//init gbuffer textures
	DX12ResourceCreationProperties gbuffertextureprops;
	DX12TextureSimple::InitResourceCreationProperties(gbuffertextureprops);
	gbuffertextureprops.resdesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	gbuffertextureprops.resdesc.Width = m_swapchain.GetSwapchainWidth();
	gbuffertextureprops.resdesc.Height = m_swapchain.GetSwapchainHeight();
	gbuffertextureprops.useclearvalue = true;
	gbuffertextureprops.optimizedclearvalue.Format = gbuffertextureprops.resdesc.Format;
	
	gbuffertextureprops.optimizedclearvalue.Color[0] = 0.0f;
	gbuffertextureprops.optimizedclearvalue.Color[1] = 0.0f;
	gbuffertextureprops.optimizedclearvalue.Color[2] = 0.0f;
	gbuffertextureprops.optimizedclearvalue.Color[3] = 0.0f;
	m_gbuffernormal.Init(m_creationdevice, gbuffertextureprops, ResourceCreationMode::COMMITED);
	D3D12_RENDER_TARGET_VIEW_DESC gbufferrtvdesc = {};
	gbufferrtvdesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	gbufferrtvdesc.Texture2D.MipSlice = 0;
	gbufferrtvdesc.Texture2D.PlaneSlice = 0;
	m_gbuffernormal.CreateRTV(m_creationdevice, gbufferrtvdesc, m_gbufferrtvheaps.GetCPUHandleOffseted(0));
	

	InitPSO();
	//gbuffer rtv heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC gbufferrtvheapdesc = {};
		gbufferrtvheapdesc.NumDescriptors = 1;
		gbufferrtvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		m_gbufferrtvheaps.Init(gbufferrtvheapdesc, m_creationdevice);
	}
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	BasicModelManager::InitCubeModel(m_creationdevice, m_cubemodel);
	m_planemodel.UploadModelDatatoBuffers();
	m_cubemodel.UploadModelDatatoBuffers();
	m_loadedmodel.UploadModelDatatoBuffers();

	m_uploadcommandlist.Reset();
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_loadedmodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_cubemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	DXASSERT(m_uploadcommandlist->Close());
}

void RayTracingApplication::InitPSO()
{
	PSOInitData psoinitdata;
	psoinitdata.type = PSOType::GRAPHICS;

	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/modeltest/ModelTestVertexShader.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/modeltest/ModelTestPixelShader.hlsl", DX12Shader::ShaderType::PS);
	psoinitdata.m_shaderstouse.push_back(vs); psoinitdata.m_shaderstouse.push_back(ps);
	DX12PSO::DefaultInitPSOData(psoinitdata);
	psoinitdata.psodesc.graphicspsodesc.PS.BytecodeLength = ps->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.PS.pShaderBytecode = ps->GetCompiledCode();
	psoinitdata.psodesc.graphicspsodesc.VS.BytecodeLength = vs->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.VS.pShaderBytecode = vs->GetCompiledCode();
	//root signature setup
	{
		{
			D3D12_ROOT_PARAMETER rootparams[1] = {};
			D3D12_ROOT_SIGNATURE_DESC& rootsigdesc = m_rootsignature.getSignatureDescforModification();
			rootsigdesc.NumParameters = 1;

			rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			rootparams[0].Constants.Num32BitValues = sizeof(XMMATRIX) / 4;
			rootparams[0].Constants.RegisterSpace = 0;
			rootparams[0].Constants.ShaderRegister = 0;
			rootsigdesc.pParameters = rootparams;

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

		m_rootsignature.Init(m_creationdevice, D3D_ROOT_SIGNATURE_VERSION_1);
		psoinitdata.psodesc.graphicspsodesc.pRootSignature = m_rootsignature.GetRootSignature();
	}
	m_pso.Init(m_creationdevice, psoinitdata);
}

void RayTracingApplication::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	m_maincameracontroller.ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);

}