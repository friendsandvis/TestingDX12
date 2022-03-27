#include"MipTestApplication.h"




MipTestApplication::MipTestApplication()
	:m_planemodel(ModelDataUploadMode::COPY)
{
}
MipTestApplication::~MipTestApplication()
{
}
void MipTestApplication::PreRenderUpdate()
{

	XMVECTOR eyepos = XMVectorSet(0.0f, 0.0f, 10.0f, 1);
	XMVECTOR focuspoint = XMVectorSet(0, 0, 0, 1);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX viewmat = XMMatrixLookAtLH(eyepos, focuspoint, up);

	float aspectratio = m_swapchain.GetSwapchainWidth() / (float)m_swapchain.GetSwapchainHeight();

	XMMATRIX projmat = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_maincamera.GetFoV()), aspectratio, 0.1f, 100.0f);
	m_maincamera.SetView(viewmat);
	m_maincamera.SetProjection(projmat);
}

void MipTestApplication::Render()
{
	m_primarycmdlist.Reset();
	//set rtv
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(m_swapchain.GetCurrentbackbufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = m_dsvdescheap.GetCPUHandlefromstart();
	m_primarycmdlist->SetPipelineState(m_pso.GetPSO());
	m_primarycmdlist->SetGraphicsRootSignature(m_rootsignature.GetRootSignature());
	XMMATRIX mvp = m_maincamera.GetMVP();
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvp, 0);
	{
		ID3D12DescriptorHeap* heapstoset[1] = {m_srvheap.GetDescHeap()};
		m_primarycmdlist->SetDescriptorHeaps(1, heapstoset);
		m_primarycmdlist->SetGraphicsRootDescriptorTable(1, m_srvheap.GetGPUHandlefromstart());
	}
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE, &dsvhandle);
	float clearvalue[4] = { 1.0f,1.0f,1.0f,1.0f };
	m_primarycmdlist->ClearRenderTargetView(rtvhandle, clearvalue, 0, nullptr);
	m_primarycmdlist->ClearDepthStencilView(dsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	{
		m_primarycmdlist->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		D3D12_INDEX_BUFFER_VIEW ibview = m_planemodel.GetIBView();
		D3D12_VERTEX_BUFFER_VIEW vbview = m_planemodel.GetVBView();
		m_primarycmdlist->IASetVertexBuffers(0, 1, &vbview);
		m_primarycmdlist->IASetIndexBuffer(&ibview);

	}

	{
		D3D12_VIEWPORT aviewport = GetViewport();

		D3D12_RECT ascissorrect = GetScissorRect();
		m_primarycmdlist->RSSetViewports(1, &aviewport);
		m_primarycmdlist->RSSetScissorRects(1, &ascissorrect);
	}
	m_primarycmdlist->DrawIndexedInstanced(m_planemodel.GetIndiciesCount(), 1, 0, 0, 0);
	DXASSERT(m_primarycmdlist->Close())
		BasicRender();
}

void MipTestApplication::InitExtras()
{
	InitPSO();
	BasicModelManager::InitPlaneModel(m_creationdevice, m_planemodel);
	m_planemodel.UploadModelDatatoBuffers();

	//init desc heaps
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvheapdesc = {};
		srvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvheapdesc.NodeMask = 0;
		srvheapdesc.NumDescriptors = 1;//1 srv
		srvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		m_srvheap.Init(srvheapdesc, m_creationdevice);
		D3D12_DESCRIPTOR_HEAP_DESC uavheapdesc = {};
		uavheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		uavheapdesc.NodeMask = 0;
		uavheapdesc.NumDescriptors = 8;//1 per mip
		uavheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_uavheap.Init(uavheapdesc, m_creationdevice);
		uavheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_uavheapupload.Init(uavheapdesc, m_creationdevice);
	}
	//init test texture
	{
		DX12ResourceCreationProperties testtexprops;
		DX12TextureSimple::InitResourceCreationProperties(testtexprops);
		testtexprops.resdesc.Width = 256;
		testtexprops.resdesc.Height = 256;
		testtexprops.resdesc.MipLevels = 8;
		testtexprops.resdesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
		testtexprops.resdesc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		m_testtexture.Init(m_creationdevice, testtexprops, ResourceCreationMode::COMMITED);
		m_testtexture.SetName(L"miptesttex");
		D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
		srvdesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
		srvdesc.Texture2D.MipLevels = testtexprops.resdesc.MipLevels;
		srvdesc.Texture2D.MostDetailedMip = 0;
		srvdesc.Texture2D.PlaneSlice = 0;
		srvdesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
		srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		m_testtexture.CreateSRV(m_creationdevice,srvdesc , m_srvheap.GetCPUHandlefromstart());
	}
	{
		//create uav for each mip of testtex
		for (UINT i = 0; i < 8; i++)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE uavhandle = m_uavheapupload.GetCPUHandleOffseted(i);
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc = {};
			uavdesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavdesc.Texture2D.MipSlice = i;
			uavdesc.Texture2D.PlaneSlice = 0;
			m_testtexture.CreateUAV(m_creationdevice, uavdesc, uavhandle);
		}
		m_creationdevice->CopyDescriptorsSimple(8, m_uavheap.GetCPUHandlefromstart(), m_uavheapupload.GetCPUHandlefromstart(), D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	m_uploadcommandlist.Reset();
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	{
		//clear testtex mips
		ID3D12DescriptorHeap* descheaptoset[1] =
		{m_uavheap.GetDescHeap()};
		m_uploadcommandlist->SetDescriptorHeaps(1, descheaptoset);
		D3D12_RESOURCE_BARRIER barrier=m_testtexture.TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		m_uploadcommandlist->ResourceBarrier(1, &barrier);
		float redclearvalue[4] = { 1.0f,0.0f,0.0f,1.0f };
		float greenclearvalue[4] = { 0.0f,1.0f,0.0f,1.0f };
		float blueclearvalue[4] = { 0.0f,0.0f,1.0f,1.0f };
		float yellowclearvalue[4] = { 1.0f,1.0f,0.0f,1.0f };
		float* clearcolour[4] = { redclearvalue,greenclearvalue,blueclearvalue,yellowclearvalue };
		for (unsigned i = 0; i < 8; i++)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE uavgpuhandle = m_uavheap.GetGPUHandleOffseted(i);
			D3D12_CPU_DESCRIPTOR_HANDLE uavcpuhandle = m_uavheapupload.GetCPUHandleOffseted(i);
			 
			m_uploadcommandlist->ClearUnorderedAccessViewFloat(uavgpuhandle, uavcpuhandle, m_testtexture.GetResource().Get(), clearcolour[i%4], 0, nullptr);
		}
		barrier = m_testtexture.TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_uploadcommandlist->ResourceBarrier(1, &barrier);
	}
	DXASSERT(m_uploadcommandlist->Close());
}

void MipTestApplication::InitPSO()
{
	PSOInitData psoinitdata;
	psoinitdata.type = PSOType::GRAPHICS;

	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/miptest/MipTestVertexShader.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/miptest/MipTestPixelShader.hlsl", DX12Shader::ShaderType::PS);
	psoinitdata.m_shaderstouse.push_back(vs); psoinitdata.m_shaderstouse.push_back(ps);
	DX12PSO::DefaultInitPSOData(psoinitdata);
	psoinitdata.psodesc.graphicspsodesc.PS.BytecodeLength = ps->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.PS.pShaderBytecode = ps->GetCompiledCode();
	psoinitdata.psodesc.graphicspsodesc.VS.BytecodeLength = vs->GetCompiledCodeSize();
	psoinitdata.psodesc.graphicspsodesc.VS.pShaderBytecode = vs->GetCompiledCode();
	//root signature setup
	{
		{
			D3D12_ROOT_PARAMETER rootparams[2] = {};
			D3D12_ROOT_SIGNATURE_DESC& rootsigdesc = m_rootsignature.getSignatureDescforModification();
			
			D3D12_DESCRIPTOR_RANGE descraanges[1];
			descraanges[0].BaseShaderRegister = 0;
			descraanges[0].RegisterSpace = 0;
			descraanges[0].OffsetInDescriptorsFromTableStart = 0;
			
			descraanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			descraanges[0].NumDescriptors = 1;

			rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			rootparams[0].Constants.Num32BitValues = sizeof(XMMATRIX) / 4;
			rootparams[0].Constants.RegisterSpace = 0;
			rootparams[0].Constants.ShaderRegister = 0;
			rootparams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootparams[1].DescriptorTable.NumDescriptorRanges = 1;
			rootparams[1].DescriptorTable.pDescriptorRanges = descraanges;
			rootparams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			rootsigdesc.pParameters = rootparams;
			rootsigdesc.NumParameters = 2;
			D3D12_STATIC_SAMPLER_DESC simplepointsampler = StaticSamplerManager::GetDefaultStaticSamplerDesc();
			rootsigdesc.NumStaticSamplers = 1;
			rootsigdesc.pStaticSamplers = &simplepointsampler;

		}

		//input assembler setup
		D3D12_INPUT_ELEMENT_DESC inputelements[2] = {0};
		inputelements[0].SemanticName = "POS";
		inputelements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputelements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputelements[0].InputSlot = 0;
		inputelements[1].SemanticName = "VUV";
		inputelements[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputelements[1].InputSlot = 0;
		inputelements[1].AlignedByteOffset = sizeof(float) * 3;//after three floats is uv

		psoinitdata.psodesc.graphicspsodesc.InputLayout.NumElements = 2;
		psoinitdata.psodesc.graphicspsodesc.InputLayout.pInputElementDescs = inputelements;

		m_rootsignature.Init(m_creationdevice, D3D_ROOT_SIGNATURE_VERSION_1);
		psoinitdata.psodesc.graphicspsodesc.pRootSignature = m_rootsignature.GetRootSignature();
	}
	m_pso.Init(m_creationdevice, psoinitdata);
}

void MipTestApplication::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{

	case WM_MOUSEWHEEL:
	{
		float wheeldelta = GET_WHEEL_DELTA_WPARAM(wParam) / 60.0f;
		float fovupdated = m_maincamera.GetFoV() - wheeldelta;
		m_maincamera.SetFov(fovupdated);

	}
	default:
		break;
	}
}