#include"DX12SkyboxtestingApplication.h"




SkyboxTestApplication::SkyboxTestApplication()
	:m_planemodel(ModelDataUploadMode::COPY),
	 m_cubemodel(ModelDataUploadMode::COPY)
{
	m_maincameracontroller.SetCameratoControl(&m_maincamera);
}
SkyboxTestApplication::~SkyboxTestApplication()
{
}
void SkyboxTestApplication::PreRenderUpdate()
{
	m_maincameracontroller.Update();
	DX12ApplicationManagerBase::PreRenderUpdate();
}

void SkyboxTestApplication::Render()
{
	m_primarycmdlist.Reset(false,true,m_frameIdx);
	//set rtv
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_frameIdx);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	m_primarycmdlist->SetPipelineState(m_pso.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_pso.GetRootSignature());
	CameraConstants camconst;
	
	XMMATRIX modelmat = XMMatrixIdentity();
	m_maincamera.SetModel(modelmat);
	camconst.mvp = m_maincamera.GetMVP();
	camconst.campos = m_maincamera.GetCamPos();
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0, sizeof(CameraConstants) / 4, &camconst, 0);
	{
		ID3D12DescriptorHeap* heapstoset[1] = { m_resourceaccessheap.GetDescHeap() };
		m_primarycmdlist->SetDescriptorHeaps(1, heapstoset);
		m_primarycmdlist->SetGraphicsRootDescriptorTable(1, m_resourceaccessheap.GetGPUHandlefromstart());
	}
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE, &dsvhandle);
	float clearvalue[4] = { 1.0f,1.0f,1.0f,1.0f };
	ClearBackBuffer(m_swapchain.GetCurrentbackbufferIndex(),m_primarycmdlist,clearvalue);
	m_primarycmdlist->ClearDepthStencilView(dsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	{
		m_primarycmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D12_INDEX_BUFFER_VIEW ibview = m_cubemodel.GetIBView();
		D3D12_VERTEX_BUFFER_VIEW vbview = m_cubemodel.GetVBView();
		m_primarycmdlist->IASetVertexBuffers(0, 1, &vbview);
		m_primarycmdlist->IASetIndexBuffer(&ibview);

	}

	{
		D3D12_VIEWPORT aviewport = GetViewport();

		D3D12_RECT ascissorrect = GetScissorRect();
		m_primarycmdlist->RSSetViewports(1, &aviewport);
		m_primarycmdlist->RSSetScissorRects(1, &ascissorrect);
	}
	m_primarycmdlist->DrawIndexedInstanced(m_cubemodel.GetIndiciesCount(), 1, 0, 0, 0);
	{
		m_primarycmdlist->SetPipelineState(m_overlaypso.GetPSO());
		m_primarycmdlist->SetGraphicsRootSignature(m_overlaypso.GetRootSignature());
		D3D12_INDEX_BUFFER_VIEW ibview = m_planemodel.GetIBView();
		D3D12_VERTEX_BUFFER_VIEW vbview = m_planemodel.GetVBView();
		m_primarycmdlist->IASetVertexBuffers(0, 1, &vbview);
		m_primarycmdlist->IASetIndexBuffer(&ibview);
		
		XMVECTOR scalevec = XMVectorSet(324.0f, 216.0f, 1.0f, 1.0f);
		modelmat=XMMatrixScalingFromVector(scalevec);
		XMMATRIX translatemat=XMMatrixTranslation(-500.0f, 200.0f, 0.0f);
		modelmat = XMMatrixMultiply(modelmat, translatemat);
		m_maincamera.SetModel(modelmat);
		camconst.mvp = m_maincamera.GetMVP(true,true);
		m_primarycmdlist->SetGraphicsRoot32BitConstants(0, sizeof(CameraConstants) / 4, &camconst, 0);
		m_primarycmdlist->DrawIndexedInstanced(m_planemodel.GetIndiciesCount(), 1, 0, 0, 0);
	}
	DXASSERT(m_primarycmdlist->Close())
		BasicRender();
}

void SkyboxTestApplication::InitExtras()
{
	D3D12_DESCRIPTOR_HEAP_DESC resourceheapdesc = {};
	resourceheapdesc.NumDescriptors = 1;
	resourceheapdesc.NumDescriptors = 1;
	resourceheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	resourceheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	m_resourceaccessheap.Init(resourceheapdesc, m_creationdevice);

	DXTexManager::LoadTexture(L"textures/skybox.dds", m_skyboxtex.GetDXImageData());
	m_skyboxtex.Init(m_creationdevice);
	m_skyboxtex.SetName(L"skybox");
	D3D12_SHADER_RESOURCE_VIEW_DESC skyboxsrvdesc = {};
	skyboxsrvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	skyboxsrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	skyboxsrvdesc.TextureCube.MipLevels = m_skyboxtex.GetTotalMipCount();
	skyboxsrvdesc.TextureCube.MostDetailedMip = 0;
	skyboxsrvdesc.TextureCube.ResourceMinLODClamp = 0.0f;
	m_skyboxtex.CreateSRV(m_creationdevice, skyboxsrvdesc, m_resourceaccessheap.GetCPUHandlefromstart());
	InitPSO();
	InitOverlayPSO();
	BasicModelManager::InitCubeModel(m_creationdevice, m_cubemodel);
	m_cubemodel.UploadModelDatatoBuffers();
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	m_planemodel.UploadModelDatatoBuffers();

	m_uploadcommandlist.Reset();
	m_cubemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_skyboxtex.UploadTexture(m_uploadcommandlist);
	DXASSERT(m_uploadcommandlist->Close());
}

void SkyboxTestApplication::InitPSO()
{
	PSOInitData psoinitdata;
	psoinitdata.type = PSOType::GRAPHICS;

	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/skybox/SkyboxTestVertexShader.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/skybox/SkyboxTestPixelShader.hlsl", DX12Shader::ShaderType::PS);
	psoinitdata.m_shaderstouse.push_back(vs); psoinitdata.m_shaderstouse.push_back(ps);
	DX12PSO::DefaultInitPSOData(psoinitdata);
	psoinitdata.psodesc.graphicspsodesc.DepthStencilState.DepthEnable = true;
	psoinitdata.psodesc.graphicspsodesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoinitdata.psodesc.graphicspsodesc.PS.BytecodeLength = ps->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.PS.pShaderBytecode = ps->GetCompiledCode();
	psoinitdata.psodesc.graphicspsodesc.VS.BytecodeLength = vs->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.VS.pShaderBytecode = vs->GetCompiledCode();
	//root signature setup
	{
		{
			D3D12_STATIC_SAMPLER_DESC asamplerdesc= StaticSamplerManager::GetDefaultStaticSamplerDesc();
			asamplerdesc.Filter = D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			D3D12_ROOT_PARAMETER rootparams[2] = {};
			D3D12_DESCRIPTOR_RANGE cubetexsrvrange = {};
			cubetexsrvrange.BaseShaderRegister = 0;
			cubetexsrvrange.NumDescriptors = 1;
			cubetexsrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			//D3D12_ROOT_SIGNATURE_DESC& rootsigdesc = m_rootsignature.getSignatureDescforModification();
			

			rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			rootparams[0].Constants.Num32BitValues = sizeof(CameraConstants) / 4;
			rootparams[0].Constants.RegisterSpace = 0;
			rootparams[0].Constants.ShaderRegister = 0;
			rootparams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootparams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
			rootparams[1].DescriptorTable.NumDescriptorRanges = 1;
			rootparams[1].DescriptorTable.pDescriptorRanges = &cubetexsrvrange;
			/*rootsigdesc.NumParameters = 2;
			rootsigdesc.pParameters = rootparams;
			rootsigdesc.NumStaticSamplers = 1;
			rootsigdesc.pStaticSamplers = &asamplerdesc;*/
			vector<D3D12_ROOT_PARAMETER> rootparamsvector;
			rootparamsvector.push_back(rootparams[0]);
			rootparamsvector.push_back(rootparams[1]);
			vector< D3D12_STATIC_SAMPLER_DESC> samplerdescvector;
			samplerdescvector.push_back(asamplerdesc);
			m_rootsignature.BuidDesc(rootparamsvector, samplerdescvector);
			psoinitdata.rootsignature = m_rootsignature;
		}

		//input assembler setup
		D3D12_INPUT_ELEMENT_DESC inputelements[2] = { 0 };
		inputelements[0].SemanticName = "POS";
		inputelements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputelements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelements[0].InputSlot = 0;
		inputelements[1].SemanticName = "PUV";
		inputelements[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputelements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelements[1].InputSlot = 0;
		inputelements[1].AlignedByteOffset = sizeof(float) * 3;//after three floats is uv

		psoinitdata.psodesc.graphicspsodesc.InputLayout.NumElements = 2;
		psoinitdata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = inputelements;
		
		//m_rootsignature.Init(m_creationdevice, D3D_ROOT_SIGNATURE_VERSION_1);
		//psoinitdata.psodesc.graphicspsodesc.pRootSignature = m_rootsignature.GetRootSignature();
	}
	m_pso.Init(m_creationdevice, psoinitdata);
}
void SkyboxTestApplication::InitOverlayPSO()
{
	PSOInitData overlaypsoinitdata;
	overlaypsoinitdata.type = PSOType::GRAPHICS;

	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/overlay/VS.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/overlay/PS.hlsl", DX12Shader::ShaderType::PS);
	overlaypsoinitdata.m_shaderstouse.push_back(vs); overlaypsoinitdata.m_shaderstouse.push_back(ps);
	DX12PSO::DefaultInitPSOData(overlaypsoinitdata);
	overlaypsoinitdata.psodesc.graphicspsodesc.DepthStencilState.DepthEnable = false;
	overlaypsoinitdata.psodesc.graphicspsodesc.PS.BytecodeLength = ps->GetCompiledCodeSize();
	overlaypsoinitdata.psodesc.graphicspsodesc.PS.pShaderBytecode = ps->GetCompiledCode();
	overlaypsoinitdata.psodesc.graphicspsodesc.VS.BytecodeLength = vs->GetCompiledCodeSize();
	overlaypsoinitdata.psodesc.graphicspsodesc.VS.pShaderBytecode = vs->GetCompiledCode();
	//root signature setup
	{
		{
			D3D12_STATIC_SAMPLER_DESC asamplerdesc = StaticSamplerManager::GetDefaultStaticSamplerDesc();
			asamplerdesc.Filter = D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			D3D12_ROOT_PARAMETER rootparams[2] = {};
			D3D12_DESCRIPTOR_RANGE cubetexsrvrange = {};
			cubetexsrvrange.BaseShaderRegister = 0;
			cubetexsrvrange.NumDescriptors = 1;
			cubetexsrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			//D3D12_ROOT_SIGNATURE_DESC& rootsigdesc = m_rootsignature.getSignatureDescforModification();


			rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			rootparams[0].Constants.Num32BitValues = sizeof(CameraConstants) / 4;
			rootparams[0].Constants.RegisterSpace = 0;
			rootparams[0].Constants.ShaderRegister = 0;
			rootparams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootparams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
			rootparams[1].DescriptorTable.NumDescriptorRanges = 1;
			rootparams[1].DescriptorTable.pDescriptorRanges = &cubetexsrvrange;
			/*rootsigdesc.NumParameters = 2;
			rootsigdesc.pParameters = rootparams;
			rootsigdesc.NumStaticSamplers = 1;
			rootsigdesc.pStaticSamplers = &asamplerdesc;*/
			vector<D3D12_ROOT_PARAMETER> rootparamsvector;
			rootparamsvector.push_back(rootparams[0]);
			vector< D3D12_STATIC_SAMPLER_DESC> samplerdescvector;
			samplerdescvector.push_back(asamplerdesc);
			m_rootsignature.BuidDesc(rootparamsvector, samplerdescvector);
			overlaypsoinitdata.rootsignature = m_rootsignature;
		}

		//input assembler setup
		D3D12_INPUT_ELEMENT_DESC inputelements[2] = { 0 };
		inputelements[0].SemanticName = "POS";
		inputelements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputelements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelements[0].InputSlot = 0;
		inputelements[1].SemanticName = "VUV";
		inputelements[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputelements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelements[1].InputSlot = 0;
		inputelements[1].AlignedByteOffset = sizeof(float) * 3;//after three floats is uv

		overlaypsoinitdata.psodesc.graphicspsodesc.InputLayout.NumElements = 2;
		overlaypsoinitdata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = inputelements;

		//m_rootsignature.Init(m_creationdevice, D3D_ROOT_SIGNATURE_VERSION_1);
		//overlaypsoinitdata.psodesc.graphicspsodesc.pRootSignature = m_rootsignature.GetRootSignature();
	}
	m_overlaypso.Init(m_creationdevice, overlaypsoinitdata);
}


void SkyboxTestApplication::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_maincameracontroller.ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);
}