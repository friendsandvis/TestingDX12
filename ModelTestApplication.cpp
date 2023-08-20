#include"ModelTestApplication.h"
//only 1 model define to be active at a time
//#define USECONFERENCEROOMCOMPOUNDMODEL
#define USESPHONZAMODEL
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
}

void ModelTestApplication::Render()
{
	m_primarycmdlist.Reset();
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
	XMMATRIX orthoproj = XMMatrixOrthographicLH(2.0F, 2.0F, -1.0f, 1.0f);
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
	
	m_loadedcompoundmodel.Draw(m_primarycmdlist,vpmat,0);
	
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
	BasicModelManager::LoadModel(m_creationdevice, "models/Sponza.gltf", m_loadedcompoundmodel, VERTEXVERSION3);
#endif//defined(USESPHONZAMODEL)
#ifdef USECUBEMODEL
	BasicModelManager::LoadModel(m_creationdevice, "models/cubes2.dae", m_loadedcompoundmodel, VERTEXVERSION2);
#endif // USECUBEMODEL


	


	
	BasicModelManager::InitTriangleModel(m_creationdevice, m_trianglemodel);
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	BasicModelManager::InitCubeModel(m_creationdevice, m_cubemodel);
	InitPSO();
	
	m_planemodel.UploadModelDatatoBuffers();
	m_cubemodel.UploadModelDatatoBuffers();
	m_loadedmodel.UploadModelDatatoBuffers();
	m_loadedcompoundmodel.UploadModelDatatoBuffers();

	m_uploadcommandlist.Reset();
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_loadedmodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_loadedcompoundmodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
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
	//psoinitdata.psodesc.graphicspsodesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	//root signature setup
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
		vector<D3D12_ROOT_PARAMETER> rootparams;
		rootparams.push_back(rootparam0);
		D3D12_ROOT_PARAMETER rootparam1 = {};
		//making unbound range so make sure it is last range
		D3D12_DESCRIPTOR_RANGE texturesrvrange = {};
		texturesrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		texturesrvrange.BaseShaderRegister = 0;
		texturesrvrange.RegisterSpace = 0;
		texturesrvrange.NumDescriptors = -1;
		rootparam1.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootparam1.DescriptorTable.NumDescriptorRanges = 1;
		rootparam1.DescriptorTable.pDescriptorRanges = &texturesrvrange;
		rootparam1.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
		rootparams.push_back(rootparam1);
		psoinitdata.rootsignature.BuidDesc(rootparams, vector<D3D12_STATIC_SAMPLER_DESC>());
		}
	}
	m_pso.Init(m_creationdevice, psoinitdata);
}

void ModelTestApplication::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	m_maincameracontroller.ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);
	
}