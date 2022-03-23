#include"ModelTestApplication.h"




ModelTestApplication::ModelTestApplication()
	:m_planemodel(ModelDataUploadMode::COPY),
	m_cubemodel(ModelDataUploadMode::COPY)
{
}
ModelTestApplication::~ModelTestApplication()
{
}
void ModelTestApplication::Update()
{
	
	XMVECTOR eyepos = XMVectorSet(0.0f, 0.0f, 10.0f, 1);
	XMVECTOR focuspoint = XMVectorSet(0, 0, 0, 1);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX viewmat = XMMatrixLookAtLH(eyepos, focuspoint, up);
	float fov = 45;
	float aspectratio = m_swapchain.GetSwapchainWidth()/(float)m_swapchain.GetSwapchainHeight();
	XMMATRIX projmat = XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspectratio, 0.1f, 100.0f);
 	m_maincamera.SetView(viewmat);
	m_maincamera.SetProjection(projmat);
}

void ModelTestApplication::Render()
{
	m_primarycmdlist.Reset();
	//set rtv
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_swapchain.GetCurrentbackbufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	m_primarycmdlist->SetPipelineState(m_pso.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_rootsignature.GetRootSignature());
	XMMATRIX mvp = m_maincamera.GetMVP();
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0,sizeof(XMMATRIX)/4, &mvp, 0);
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE,&dsvhandle);
	float clearvalue[4] = {1.0f,1.0f,1.0f,1.0f};
	m_primarycmdlist->ClearRenderTargetView(rtvhandle, clearvalue, 0, nullptr);
	m_primarycmdlist->ClearDepthStencilView(dsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	{
		m_primarycmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		D3D12_INDEX_BUFFER_VIEW ibview=m_cubemodel.GetIBView();
		D3D12_VERTEX_BUFFER_VIEW vbview = m_cubemodel.GetVBView();
		m_primarycmdlist->IASetVertexBuffers(0, 1, &vbview);
		m_primarycmdlist->IASetIndexBuffer( &ibview);
		
	}
	
	{
		D3D12_VIEWPORT aviewport = GetViewport();

		D3D12_RECT ascissorrect = GetScissorRect();
		m_primarycmdlist->RSSetViewports(1, &aviewport);
		m_primarycmdlist->RSSetScissorRects(1, &ascissorrect);
	}
	m_primarycmdlist->DrawIndexedInstanced(m_cubemodel.GetIndiciesCount(), 1, 0, 0, 0);
	DXASSERT(m_primarycmdlist->Close())
	BasicRender();
}

void ModelTestApplication::InitExtras()
{
	InitPSO();
	BasicModelManager::InitPlaneModel(m_creationdevice,m_planemodel);
	BasicModelManager::InitCubeModel(m_creationdevice, m_cubemodel);
	m_planemodel.UploadModelDatatoBuffers();
	m_cubemodel.UploadModelDatatoBuffers();

	m_uploadcommandlist.Reset();
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_cubemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	DXASSERT(m_uploadcommandlist->Close());
}

void ModelTestApplication::InitPSO()
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
		D3D12_INPUT_ELEMENT_DESC inputelements[2] = { 0 };
		inputelements[0].SemanticName = "POS";
		inputelements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputelements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelements[0].InputSlot = 0;
		inputelements[1].SemanticName = "POSCOL";
		inputelements[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputelements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelements[1].InputSlot = 0;
		inputelements[1].AlignedByteOffset = sizeof(float)*3;//after three floats is colour
		
		psoinitdata.psodesc.graphicspsodesc.InputLayout.NumElements = 2;
		psoinitdata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = inputelements;

		m_rootsignature.Init(m_creationdevice, D3D_ROOT_SIGNATURE_VERSION_1);
		psoinitdata.psodesc.graphicspsodesc.pRootSignature = m_rootsignature.GetRootSignature();
	}
	m_pso.Init(m_creationdevice, psoinitdata);
}

void ModelTestApplication::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_XBUTTONDOWN:
	default:
		break;
	}
}