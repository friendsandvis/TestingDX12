#include"ModelTestApplication.h"




ModelTestApplication::ModelTestApplication()
	:m_planemodel(ModelDataUploadMode::COPY),
	m_cubemodel(ModelDataUploadMode::COPY),
	m_loadedmodel(ModelDataUploadMode::COPY),
	m_trianglemodel(ModelDataUploadMode::COPY)
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
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_swapchain.GetCurrentbackbufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	m_primarycmdlist->SetPipelineState(m_pso.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_rootsignature.GetRootSignature());
	//XMMATRIX mvp = m_maincamera.GetMVP();
	XMMATRIX orthoproj = XMMatrixOrthographicLH(2.0F, 2.0F, -100.0f, 100.0f);
	XMMATRIX model = XMMatrixIdentity();
	XMMATRIX mvp = XMMatrixMultiply(model,orthoproj);
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0,sizeof(XMMATRIX)/4, &mvp, 0);
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE,&dsvhandle);
	float clearvalue[4] = {1.0f,1.0f,1.0f,1.0f};
	m_primarycmdlist->ClearRenderTargetView(rtvhandle, clearvalue, 0, nullptr);
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
	m_trianglemodel.Draw(m_primarycmdlist,vpmat);
	DXASSERT(m_primarycmdlist->Close())
	BasicRender();
}

void ModelTestApplication::InitExtras()
{
	BasicModelManager::LoadModel(m_creationdevice,"models/cube.dae",m_loadedmodel,VERTEXVERSION2);
	float aspectratio = m_swapchain.GetSwapchainWidth() / (float)m_swapchain.GetSwapchainHeight();
	BasicModelManager::InitTriangleModel(m_creationdevice, m_trianglemodel);
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	BasicModelManager::InitCubeModel(m_creationdevice, m_cubemodel);
	InitPSO();
	
	m_planemodel.UploadModelDatatoBuffers();
	m_cubemodel.UploadModelDatatoBuffers();
	m_loadedmodel.UploadModelDatatoBuffers();

	m_uploadcommandlist.Reset();
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_loadedmodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_cubemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_trianglemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	DXASSERT(m_uploadcommandlist->Close());
}

void ModelTestApplication::InitPSO()
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputelements;
	DXVertexManager::BuildDefaultInputelementdesc(inputelements, m_trianglemodel.GetVertexVersionUsed());
	PSOInitData psoinitdata;
	psoinitdata.type = PSOType::GRAPHICS;

	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/modeltest/ModelTestVertexShader2D.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/modeltest/ModelTestPixelShader2D.hlsl", DX12Shader::ShaderType::PS);
	psoinitdata.m_shaderstouse.push_back(vs); psoinitdata.m_shaderstouse.push_back(ps);
	DX12PSO::DefaultInitPSOData(psoinitdata);
	psoinitdata.psodesc.graphicspsodesc.PS.BytecodeLength = ps->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.PS.pShaderBytecode = ps->GetCompiledCode();
	psoinitdata.psodesc.graphicspsodesc.VS.BytecodeLength = vs->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.VS.pShaderBytecode = vs->GetCompiledCode();
	//psoinitdata.psodesc.graphicspsodesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	//root signature setup
	{
		 {
			D3D12_ROOT_PARAMETER rootparams[1] = {};
			D3D12_ROOT_SIGNATURE_DESC&rootsigdesc=m_rootsignature.getSignatureDescforModification();
			rootsigdesc.NumParameters = 1;
			
			rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			rootparams[0].Constants.Num32BitValues = sizeof(XMMATRIX) / 4;
			rootparams[0].Constants.RegisterSpace=0;
			rootparams[0].Constants.ShaderRegister = 0;
			rootsigdesc.pParameters = rootparams;

		}

		//input assembler setup
		 
		psoinitdata.psodesc.graphicspsodesc.InputLayout.NumElements = (UINT)inputelements.size();
		psoinitdata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = inputelements.data();

		m_rootsignature.Init(m_creationdevice, D3D_ROOT_SIGNATURE_VERSION_1);
		psoinitdata.psodesc.graphicspsodesc.pRootSignature = m_rootsignature.GetRootSignature();
	}
	//raster setup
	psoinitdata.psodesc.graphicspsodesc.RasterizerState.DepthClipEnable = true;
	
	m_pso.Init(m_creationdevice, psoinitdata);
}

void ModelTestApplication::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	m_maincameracontroller.ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);
	
}