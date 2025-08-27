#include"ModelTestApplication.h"
//only 1 model define to be active at a time
//#define USECONFERENCEROOMCOMPOUNDMODEL
//#define USETESTBASICMODELCUBE
#define USESPHONZAMODEL
//#define USEREVOLVERMODEL
//#define USECUBEMODEL


ModelTestApplication::ModelTestApplication()
	:m_planemodel(ModelDataUploadMode::COPY),
	m_cubemodel(ModelDataUploadMode::COPY),
	m_loadedmodel(ModelDataUploadMode::COPY),
	m_trianglemodel(ModelDataUploadMode::COPY),
	m_loadedcompoundmodel(ModelDataUploadMode::COPY)
{
	m_maincameracontroller.SetCameratoControl(&m_maincamera);
}
ModelTestApplication::~ModelTestApplication()
{
	
}
void ModelTestApplication::PreRenderUpdate()
{
	m_maincameracontroller.Update();
	DX12ApplicationManagerBase::PreRenderUpdate();
}

void ModelTestApplication::Render()
{
	m_primarycmdlist.Reset(false, true, m_frameIdx);
	bool uploadModelTextureData = false;
#if defined(USESPHONZAMODEL) || defined(USEREVOLVERMODEL)
	uploadModelTextureData = true;
#endif //defined(USESPHONZAMODEL) || defined(USEREVOLVERMODEL)


	
	//upload compoundmodel textures over frames or all at once
	const bool loadModelTextureDataOverFrames = false;
	if (uploadModelTextureData)
	{
		if (loadModelTextureDataOverFrames)
		{
			m_loadedcompoundmodel.UploadCurrentFrameModelTextureData(m_creationdevice, m_primarycmdlist);
		}
		else
		{
			m_loadedcompoundmodel.UploadAllModelTextureData(m_creationdevice, m_primarycmdlist);
		}
	}
	//set rtv
	UINT currentbackbufferidx=m_swapchain.GetCurrentbackbufferIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(currentbackbufferidx);
	D3D12_RESOURCE_BARRIER barrier=m_swapchain.TransitionBackBuffer(currentbackbufferidx, D3D12_RESOURCE_STATE_RENDER_TARGET);
	if (DXUtils::IsBarrierSafeToExecute(barrier))
	{
		m_primarycmdlist->ResourceBarrier(1, &barrier);
	}
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	m_primarycmdlist->SetPipelineState(m_pso.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_pso.GetRootSignature());
	//XMMATRIX mvp = m_maincamera.GetMVP();
	XMMATRIX orthoproj = XMMatrixOrthographicLH(2.0f, 2.0f, -1.0f, 1.0f);
	XMMATRIX model = XMMatrixIdentity();
	XMMATRIX mvp = XMMatrixMultiply(model,orthoproj);
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0,sizeof(XMMATRIX)/4, &mvp, 0);
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE,&dsvhandle);
	float clearvalue[4] = {1.0f,1.0f,1.0f,1.0f};
	ClearBackBuffer(currentbackbufferidx,m_primarycmdlist, clearvalue);
	m_primarycmdlist->ClearDepthStencilView(dsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	{
		m_primarycmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		
	}
	
	{
		D3D12_VIEWPORT aviewport = GetViewport();

		D3D12_RECT ascissorrect = GetScissorRect();
		m_primarycmdlist->RSSetViewports(1, &aviewport);
		m_primarycmdlist->RSSetScissorRects(1, &ascissorrect);
	}
	XMMATRIX vpmat = m_maincamera.GetVP();
	//m_trianglemodel.Draw(m_primarycmdlist,vpmat);
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &vpmat, 0);
	//a specialized way to test out model's material(diffuse textures) 
	if(m_loadedcompoundmodel.SupportMaterial())
	{
		DX12DESCHEAP& loadedcompoundmodelmatsrvheap = m_loadedcompoundmodel.GetResourceviewHeap();
		ID3D12DescriptorHeap* heapstoset[1] = { loadedcompoundmodelmatsrvheap.GetDescHeap() };
		m_primarycmdlist->SetDescriptorHeaps(1, heapstoset);
		m_primarycmdlist->SetGraphicsRootDescriptorTable(1, loadedcompoundmodelmatsrvheap.GetGPUHandleOffseted(0));
	}
#ifdef USETESTBASICMODELCUBE
	{
		m_cubemodel.Draw(m_primarycmdlist, vpmat, 0, 2, true, true, true);
	}
	DXASSERT(m_primarycmdlist->Close())
		BasicRender();
	return;
#endif
	//we draw opaque non opaque with diffrent [pso only if compound model supports nonopaque otherwise all rendered by default pso.
	if (m_loadedcompoundmodel.SupportNonOpaqueMaterial())
	{
		//draw opaque models only with opaque pso and switch to alpha blending pso to render non opaque data
		m_loadedcompoundmodel.Draw(m_primarycmdlist, vpmat, 0, 2, true, false);
		m_primarycmdlist->SetPipelineState(m_pso_alphablending.GetPSO());
		m_primarycmdlist->SetGraphicsRootSignature(m_pso_alphablending.GetRootSignature());
		m_loadedcompoundmodel.Draw(m_primarycmdlist, vpmat, 0, 2, false, true);
	}
	else
	{
		m_loadedcompoundmodel.Draw(m_primarycmdlist, vpmat, 0, 2, true, true);
	}
	
	DXASSERT(m_primarycmdlist->Close())
	BasicRender();
}

void ModelTestApplication::InitExtras()
{
	BasicModelManager::LoadModel(m_creationdevice,"models/cube.dae",m_loadedmodel,VERTEXVERSION2);
#ifdef USECONFERENCEROOMCOMPOUNDMODEL
	float scalefactor = 0.01f;
	XMMATRIX scalemat = XMMatrixScalingFromVector(XMVectorSet(scalefactor, scalefactor, scalefactor, 1.0f));
	BasicModelManager::LoadModel(m_creationdevice, "models/conference.obj", m_loadedcompoundmodel, VERTEXVERSION2);
	m_loadedcompoundmodel.Extratransform(scalemat);
#endif // USECONFERENCEROOMCOMPOUNDMODEL
	
	
#if defined(USESPHONZAMODEL)
	float scalefactor = 1.0f;
	XMMATRIX scalemat = XMMatrixScalingFromVector(XMVectorSet(scalefactor, scalefactor, scalefactor, 1.0f));
	BasicModelManager::LoadModel(m_creationdevice, "models/Sponza.gltf", m_loadedcompoundmodel, VERTEXVERSION3, L"textures/modeltextures/sponza/",true);
#endif//defined(USESPHONZAMODEL)
#ifdef USECUBEMODEL
	BasicModelManager::LoadModel(m_creationdevice, "models/cubes2.dae", m_loadedcompoundmodel, VERTEXVERSION2);
#endif // USECUBEMODEL
#if defined(USEREVOLVERMODEL)
	float scalefactor = 0.15f;
	XMMATRIX scalemat = XMMatrixScalingFromVector(XMVectorSet(scalefactor, scalefactor, scalefactor, 1.0f));
	BasicModelManager::LoadModel(m_creationdevice, "models/revolver/scene.gltf", m_loadedcompoundmodel, VERTEXVERSION3, L"textures/modeltextures/revolver/", true);
	//revolver model has albedo as png but it is not expected to be rendered by alphablending.
	m_loadedcompoundmodel.m_supportNonOpaqueMaterial = false;

	m_loadedcompoundmodel.Extratransform(scalemat);
#endif//defined(USEREVOLVERMODEL)


	


	
	BasicModelManager::InitTriangleModel(m_creationdevice, m_trianglemodel);
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	BasicModelManager::InitCubeModel(m_creationdevice, m_cubemodel,VertexVersion::VERTEXVERSION3);
	Model::MaterialConstants cubeMaterialConstants = m_cubemodel.GetMaterialConstants();
	cubeMaterialConstants.texsrvidx = 0;//arbatary unused for test cube model.
	m_cubemodel.SetMaterialConstants(cubeMaterialConstants);
	InitPSO();
	
	m_planemodel.UploadModelDatatoBuffers();
	m_cubemodel.UploadModelDatatoBuffers();
	m_loadedmodel.UploadModelDatatoBuffers();
	m_loadedcompoundmodel.UploadModelDatatoBuffers();

	m_uploadcommandlist.Reset();
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_loadedmodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	//m_loadedcompoundmodel.UploadModelDataDefaultTexture(m_uploadcommandlist);
	m_loadedcompoundmodel.UploadData(m_uploadcommandlist);
	//m_loadedcompoundmodel.UploadModelTextureData(m_uploadcommandlist);
	m_cubemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_trianglemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	DXASSERT(m_uploadcommandlist->Close());
}

void ModelTestApplication::InitPSO()
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputelements;
	DXVertexManager::BuildDefaultInputelementdesc(inputelements, VERTEXVERSION3);
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
	//psoinitdata.psodesc.graphicspsodesc.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT;
	//root signature setup
	vector < D3D12_DESCRIPTOR_RANGE> descrangestouserootparam1;
	vector<D3D12_ROOT_PARAMETER> rootparams;
	{
		 

		//input assembler setup
		 
		psoinitdata.psodesc.graphicspsodesc.InputLayout.NumElements = (UINT)inputelements.size();
		psoinitdata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = inputelements.data();
		
		//psoinitdata.psodesc.graphicspsodesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		{D3D12_ROOT_PARAMETER rootparam0 = {};
		rootparam0.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		rootparam0.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootparam0.Constants.Num32BitValues = sizeof(ShaderTransformConstants_General) / 4;
		rootparam0.Constants.RegisterSpace = 0;
		rootparam0.Constants.ShaderRegister = 0;
		rootparams.push_back(rootparam0);
		D3D12_ROOT_PARAMETER rootparam1 = {};
		//making mat table srv range
		D3D12_DESCRIPTOR_RANGE materialtablerange = {};
		materialtablerange.OffsetInDescriptorsFromTableStart = 0;
		materialtablerange.NumDescriptors = 1;
		materialtablerange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		materialtablerange.RegisterSpace = 1;
		materialtablerange.BaseShaderRegister = 0;
		//making unbound range so make sure it is last range
		D3D12_DESCRIPTOR_RANGE texturesrvrange = {};
		texturesrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		texturesrvrange.BaseShaderRegister = 1;
		texturesrvrange.RegisterSpace = 0;
		texturesrvrange.NumDescriptors = -1;
		texturesrvrange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		//do not know why order of range is important here else issue in unbound range(all texture).
		descrangestouserootparam1.push_back(texturesrvrange);
		descrangestouserootparam1.push_back(materialtablerange);
		
		
		
		
		
		
		
		rootparam1.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootparam1.DescriptorTable.NumDescriptorRanges = descrangestouserootparam1.size();
		rootparam1.DescriptorTable.pDescriptorRanges = descrangestouserootparam1.data();
		rootparam1.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
		rootparams.push_back(rootparam1);
		D3D12_ROOT_PARAMETER rootparam2 = {};
		rootparam2.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		rootparam2.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootparam2.Constants.Num32BitValues = sizeof(Model::MaterialConstants) / 4;
		rootparam2.Constants.RegisterSpace = 1;
		rootparam2.Constants.ShaderRegister = 1;
		rootparams.push_back(rootparam2);
		D3D12_ROOT_PARAMETER rootparam3 = {};

		
		vector<D3D12_STATIC_SAMPLER_DESC> staticsamplers;
		{
			D3D12_STATIC_SAMPLER_DESC simplesampler = {};
			simplesampler.Filter = D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			simplesampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			simplesampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			simplesampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			simplesampler.MipLODBias = 0;
			simplesampler.MaxAnisotropy = 0;
			simplesampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			simplesampler.BorderColor = D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			simplesampler.MinLOD = 0.0f;
			simplesampler.MaxLOD = D3D12_FLOAT32_MAX;
			simplesampler.ShaderRegister = 0;
			simplesampler.RegisterSpace = 0;
			simplesampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			staticsamplers.push_back(simplesampler);
		}
		psoinitdata.rootsignature.BuidDesc(rootparams, staticsamplers);
		}
	}
	m_pso.Init(m_creationdevice, psoinitdata);
	{
		//for alpha blending version of pso set alpha blending properties to basic pso settings.
		//blendstate setup
		DX12PSO::SetPSOData_DefaultAlphaSettings(psoinitdata, DX12PSO::PSOBlendingSetting::STRAIGHT_ALPHA);
		/*for (size_t i = 0; i < 8; i++)
		{
			//same for all 8 rtvs
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
		}*/
		//turn off z write for alphablending
		psoinitdata.psodesc.graphicspsodesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		//we do not need to rehold the shaders here in blending pso so clear held shaders list(already passed and hence managed in non alpha blending version of pso and shaders used for both psos are same hence not required to pass the same list of shaders to be managed to alpha version of pso as well).
		psoinitdata.m_shaderstouse.clear();
		m_pso_alphablending.Init(m_creationdevice, psoinitdata);
	}
}

void ModelTestApplication::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	m_maincameracontroller.ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);
	
}