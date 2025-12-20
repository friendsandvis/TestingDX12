#include"LightingTestApplication.h"
//only 1 model define to be active at a time
//#define USECONFERENCEROOMCOMPOUNDMODEL
#define USETESTBASICMODELCUBE
//#define USESPHONZAMODEL
//#define USEREVOLVERMODEL
//#define USECUBEMODEL
#define USECUSTOMMATERIALTEXTURE


LightingTestApplication::LightingTestApplication()
	:m_planemodel(ModelDataUploadMode::COPY),
	m_cubemodel_simpleTesting(ModelDataUploadMode::COPY),
	m_loadedmodel(ModelDataUploadMode::COPY),
	m_trianglemodel(ModelDataUploadMode::COPY),
	m_loadedcompoundmodel(ModelDataUploadMode::COPY),
	m_cubemodel_simpleLight(ModelDataUploadMode::COPY),
	m_boxtextureDiffuse(nullptr),
	m_boxtextureSpec(nullptr)
{
	m_boxMaterialGPUData.diffusetexidx = 0;
	m_boxMaterialGPUData.normaltexidx = 0;
	m_boxMaterialGPUData.roughnesstexidx = 0;
	m_maincameracontroller.SetCameratoControl(&m_maincamera);
	m_imguiAllowed = true;
	//since we are using imgui in this app so initially mouse movement is turned off
	m_Imgui_mousecontrol_camera = false;
#ifdef TESTLIGHTTYPE_POINT
	m_TestLightProperties.lightPos = XMFLOAT3(1.2f, 1.0f, 2.0f);
	m_TestLightProperties.lightCol = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_TestLightProperties.data1 = 1.0f;
	m_TestLightProperties.data2 = 2.0f;
#elif defined(TESTLIGHTTYPE_DIRECTION)
	m_TestLightProperties.lightDir = XMFLOAT3(0.2f, 1.0f,0.3f);
	m_TestLightProperties.lightCol = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_TestLightProperties.numLocallight = static_cast<float>(m_localLights.size());
	m_TestLightProperties.usedirectionallight = (float)m_useDirectionalLighting;
#endif // TESTLIGHTTYPE_POINT

	//default init local light primary
	{
		pointLightprimary.lightCol = XMFLOAT4(1.0f, 1.0f, 1.0f,3.0f);
		pointLightprimary.lightPos = XMFLOAT4(1.2f, 1.0f, 2.0f,4.0f);
	}
}
LightingTestApplication::~LightingTestApplication()
{
	if (m_boxtextureDiffuse != nullptr)
	{
		delete m_boxtextureDiffuse;
	}
	if (m_boxtextureSpec != nullptr)
	{
		delete m_boxtextureSpec;
	}
}
void LightingTestApplication::PreRenderUpdate()
{
	m_maincameracontroller.AllowMouseMovementProcessing(m_Imgui_mousecontrol_camera);
	m_maincameracontroller.Update();
	DX12ApplicationManagerBase::PreRenderUpdate();
}

void LightingTestApplication::Render()
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
	//upload textures custom
#ifdef USECUSTOMMATERIALTEXTURE
	m_boxtextureDiffuse->UploadTexture(m_primarycmdlist);
	m_boxtextureSpec->UploadTexture(m_primarycmdlist);
#endif
	//update locallightbuffer forced per frame for quick test refresh content of local light buffer too
	{
		m_localLights.clear();
		m_localLights.push_back(pointLightprimary);
		m_localLightdatabufferNeedUpdate = true;
		UpdateLocalLightBufferData();
	}
	//set rtv
	UINT currentbackbufferidx = m_swapchain.GetCurrentbackbufferIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = m_rtvdescheap.GetCPUHandleOffseted(currentbackbufferidx);
	D3D12_RESOURCE_BARRIER barrier = m_swapchain.TransitionBackBuffer(currentbackbufferidx, D3D12_RESOURCE_STATE_RENDER_TARGET);
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
	XMMATRIX mvp = XMMatrixMultiply(model, orthoproj);
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvp, 0);
	m_primarycmdlist->OMSetRenderTargets(1, &rtvhandle, FALSE, &dsvhandle);
	float clearvalue[4] = { 0.0f,0.1f,0.25f,1.0f };
	ClearBackBuffer(currentbackbufferidx, m_primarycmdlist, clearvalue);
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
	CameraMatriciesData camMatData = m_maincamera.GetVPSeperate();
	XMMATRIX vpmat = m_maincamera.GetVP();
	//m_trianglemodel.Draw(m_primarycmdlist,vpmat);
	m_primarycmdlist->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &vpmat, 0);
	//a specialized way to test out model's material(diffuse textures) 
	if (m_loadedcompoundmodel.SupportMaterial())
	{
		DX12DESCHEAP& loadedcompoundmodelmatsrvheap = m_loadedcompoundmodel.GetResourceviewHeap();
		ID3D12DescriptorHeap* heapstoset[1] = { loadedcompoundmodelmatsrvheap.GetDescHeap() };
		m_primarycmdlist->SetDescriptorHeaps(1, heapstoset);
		m_primarycmdlist->SetGraphicsRootDescriptorTable(1, loadedcompoundmodelmatsrvheap.GetGPUHandleOffseted(0));
	}
	
	//------------------------------- draw test light representing cube use same PSO for now
	{
		//setup position and scale first then render with right shader properties.
		XMFLOAT4 simplelightCube_scale = { 0.25f,0.25,0.25f,1.0f };
		XMFLOAT4 simplelightCube_rotationAxis = { 1.0f,0.0,0.0f,1.0f };
		XMFLOAT4 simplelightCube_translate = { pointLightprimary.lightPos.x,pointLightprimary.lightPos.y,pointLightprimary.lightPos.z,1.0f };
		m_cubemodel_simpleLight.SetTransformation(simplelightCube_scale, simplelightCube_rotationAxis, 0.0f, simplelightCube_translate);
		{
			CustomMaterial customMaterialsimplelight_cube = {};
			customMaterialsimplelight_cube.ambient = { pointLightprimary.lightCol.x,pointLightprimary.lightCol.y,pointLightprimary.lightCol.z,1.0f };//light colour here
			customMaterialsimplelight_cube.usecustomMaterial = 1.0f;
			customMaterialsimplelight_cube.lightingMode = static_cast<unsigned int>(LIGHTINGMODE::ALBEDOONLY);
			DirectX::XMStoreFloat4(&customMaterialsimplelight_cube.viewPos, m_maincamera.GetCamPos());
			m_primarycmdlist->SetGraphicsRoot32BitConstants(3, sizeof(customMaterialsimplelight_cube) / 4, &customMaterialsimplelight_cube, 0);
			//light properties buffer not needed but set for completenesss sake change as we are avoiding new set of shader for light represent rendering in this app.
			m_TestLightProperties.usedirectionallight = m_useDirectionalLighting;
			m_primarycmdlist->SetGraphicsRoot32BitConstants(4, sizeof(m_TestLightProperties) / 4, &m_TestLightProperties, 0);
			//light properties directly passed to material & used representation cube transform
		}
		m_cubemodel_simpleLight.Draw(m_primarycmdlist, vpmat, 0, 2, true, true, true);
	}
	CustomMaterial customMaterial = {};
	customMaterial.usecustomMaterial = 0.0f;
#ifdef USETESTBASICMODELCUBE
	customMaterial.usecustomMaterial = 1.0f;
	customMaterial.lightingMode = static_cast<unsigned int>(LIGHTINGMODE::COMPLETELIGHTING_BASIC);
#endif // USETESTBASICMODELCUBE

#ifdef USECUSTOMMATERIALTEXTURE
	customMaterial.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);//XMFLOAT4(1.0f, 0.5f, 0.31f, 1.0f);
	customMaterial.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	customMaterial.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	customMaterial.specularValue = 32.0f;
	customMaterial.useMaterialTextures = 1.0f;

#else
	customMaterial.ambient = XMFLOAT4(1.0f, 0.5f, 0.31f, 1.0f);
	customMaterial.diffuse = XMFLOAT4(1.0f, 0.5f, 0.31f, 1.0f);
	customMaterial.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	customMaterial.specularValue = 64.0f;
	customMaterial.useMaterialTextures = 0.0f;
#endif // USECUSTOMMATERIALTEXTURE

	
	DirectX::XMStoreFloat4(&customMaterial.viewPos, m_maincamera.GetCamPos());
	m_primarycmdlist->SetGraphicsRoot32BitConstants(3, sizeof(customMaterial) / 4, &customMaterial, 0);
	m_primarycmdlist->SetGraphicsRoot32BitConstants(4, sizeof(m_TestLightProperties) / 4, &m_TestLightProperties, 0);
#ifdef USETESTBASICMODELCUBE
	{
		//set custom texture material table and heaps for same
		DX12DESCHEAP& custommatsrvheap = m_SRVHeap;
		ID3D12DescriptorHeap* heapstoset[1] = { custommatsrvheap.GetDescHeap() };
		m_primarycmdlist->SetDescriptorHeaps(1, heapstoset);
		m_primarycmdlist->SetGraphicsRootDescriptorTable(1, custommatsrvheap.GetGPUHandleOffseted(0));
		//customMaterial constants for texture based material use
		Model::MaterialConstants modelcustomMaterialConstants = {};
		modelcustomMaterialConstants.texsrvidx = 0;
		m_primarycmdlist->SetGraphicsRoot32BitConstants(2, sizeof(modelcustomMaterialConstants) / 4, &modelcustomMaterialConstants, 0);
	}
	{
		//rendering multiple cubes instanced render
			m_cubemodel_simpleTesting.Draw(m_primarycmdlist, camMatData, 0, 2, true, true, true,NUMCUBESTORENDER);
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

void LightingTestApplication::InitExtras()
{
	// local light init
	{
		m_localLights.push_back(pointLightprimary);
	}
	//init instance data
	{
		for (int i = 0; i < NUMCUBESTORENDER; i++)
		{
			CubeInstanceData instanceData{};
			XMVECTOR rotAxis{ 0.0f,1.0f,0.0f };
			XMVECTOR position = XMVectorSet(0.0f, i * 50.0f, 0.0f, 1.0f);
			instanceData.modelMat = DXUtils::GetTransformationMatrix(1.0f, rotAxis, 0.0f, position);
			m_instanceData.push_back(instanceData);
		}
	}
	//texture map material custom
	{
		//the resource heap
		UINT actualSRVtoallocate = 5;// 1 texmat data 1 for locallight data buffer, 1 for testcube instance data and 2 tex(forming unbound tex views
		D3D12_DESCRIPTOR_HEAP_DESC texsrvheapdesc = {};
		texsrvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		texsrvheapdesc.NumDescriptors = actualSRVtoallocate;
		texsrvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_SRVHeap.Init(texsrvheapdesc, m_creationdevice);
#ifdef USECUSTOMMATERIALTEXTURE
		{
			//custom texture init
			m_boxtextureDiffuse = new DXTexture(L"boxdiffuse.png");
			{
				wstring texpath = L"textures/lightingtest/";
				wstring texfullpath = texpath + m_boxtextureDiffuse->GetExternalTextureFileName();
				DXTexManager::LoadTexture(texfullpath.c_str(), m_boxtextureDiffuse->GetDXImageData(),true);
			}
			{
				//init and create srv for tex
				m_boxtextureDiffuse->Init(m_creationdevice);
				D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
				srvdesc.Texture2D.MipLevels = m_boxtextureDiffuse->GetTotalMipCount();
				srvdesc.Format = m_boxtextureDiffuse->GetDXImageData().m_imagemetadata.format;
				srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				m_boxtextureDiffuse->CreateSRV(m_creationdevice, srvdesc, m_SRVHeap.GetCPUHandleOffseted(3));
				m_boxMaterialGPUData.diffusetexidx = 0;
			}
			m_boxtextureSpec = new DXTexture(L"boxspec.png");
			{
				wstring texpath = L"textures/lightingtest/";
				wstring texfullpath = texpath + m_boxtextureSpec->GetExternalTextureFileName();
				DXTexManager::LoadTexture(texfullpath.c_str(), m_boxtextureSpec->GetDXImageData(),true);
			}
			{
				//init and create srv for tex
				m_boxtextureSpec->Init(m_creationdevice);
				D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
				srvdesc.Texture2D.MipLevels = m_boxtextureSpec->GetTotalMipCount();
				srvdesc.Format = m_boxtextureSpec->GetDXImageData().m_imagemetadata.format;
				srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				m_boxtextureSpec->CreateSRV(m_creationdevice, srvdesc, m_SRVHeap.GetCPUHandleOffseted(4));
				m_boxMaterialGPUData.roughnesstexidx = 1;
			}
		}
#endif
		//the tex data buffer
		DX12ResourceCreationProperties materialtexdataBufferprops;
		DX12Buffer::InitResourceCreationProperties(materialtexdataBufferprops);
		//just 1 matdata block here
		materialtexdataBufferprops.resdesc.Width = sizeof(MaterialDataGPU) * 1;
		materialtexdataBufferprops.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		m_materialTexDatabuffer.Init(m_creationdevice, materialtexdataBufferprops, ResourceCreationMode::COMMITED);
		m_materialTexDatabuffer.SetName(L"custommaterialtablebuffer");
		//cam const data buffer
		DX12ResourceCreationProperties camConstDataBufferProps;
		DX12Buffer::InitResourceCreationProperties(camConstDataBufferProps);
		//just 1 cam const block here
		camConstDataBufferProps.resdesc.Width = sizeof(ShaderTransformConstants_GeneralComplete) * 1;
		camConstDataBufferProps.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		m_CamConstBuffer.Init(m_creationdevice, camConstDataBufferProps, ResourceCreationMode::COMMITED);
		m_CamConstBuffer.SetName(L"camConstdatabuffer");
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC canConstbuffersrvdesc = {};
			canConstbuffersrvdesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
			canConstbuffersrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			canConstbuffersrvdesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			canConstbuffersrvdesc.Buffer.FirstElement = 0;
			canConstbuffersrvdesc.Buffer.NumElements = static_cast<UINT>(1);
			canConstbuffersrvdesc.Buffer.StructureByteStride = sizeof(ShaderTransformConstants_GeneralComplete);
			m_materialTexDatabuffer.CreateSRV(m_creationdevice, canConstbuffersrvdesc, m_SRVHeap.GetCPUHandleOffseted(1));
		}
		//create material table srv in the heap created
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC materialtablebuffersrvdesc = {};
			materialtablebuffersrvdesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
			materialtablebuffersrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			materialtablebuffersrvdesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			materialtablebuffersrvdesc.Buffer.FirstElement = 0;
			materialtablebuffersrvdesc.Buffer.NumElements = static_cast<UINT>(1);
			materialtablebuffersrvdesc.Buffer.StructureByteStride = sizeof(MaterialDataGPU);
			m_materialTexDatabuffer.CreateSRV(m_creationdevice, materialtablebuffersrvdesc, m_SRVHeap.GetCPUHandleOffseted(0));
			
		}
		{
			//upload box custom gpu data
			BufferMapParams mattablewriteparams = {};
			mattablewriteparams.range.Begin = 0;
			mattablewriteparams.range.End = m_materialTexDatabuffer.GetSize();
			void* mattablebuffmapped = m_materialTexDatabuffer.Map(mattablewriteparams);
			memcpy(mattablebuffmapped, &m_boxMaterialGPUData, m_materialTexDatabuffer.GetSize());
			m_materialTexDatabuffer.UnMap(mattablewriteparams);
		}
		//locallight data buffer
		DX12ResourceCreationProperties locallightDataBufferProps;
		DX12Buffer::InitResourceCreationProperties(locallightDataBufferProps);
		locallightDataBufferProps.resdesc.Width = sizeof(PointLight) * static_cast<UINT64>(m_localLights.size());
		locallightDataBufferProps.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		m_localLightsBuffer.Init(m_creationdevice, locallightDataBufferProps, ResourceCreationMode::COMMITED);
		m_localLightsBuffer.SetName(L"locallightdatabuffer");
		//instance data buffer
		DX12ResourceCreationProperties instanceDataBufferProps;
		DX12Buffer::InitResourceCreationProperties(instanceDataBufferProps);
		instanceDataBufferProps.resdesc.Width = sizeof(CubeInstanceData) * static_cast<UINT64>(m_instanceData.size());
		instanceDataBufferProps.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		m_testCubeInstanceDataBuffer.Init(m_creationdevice, instanceDataBufferProps, ResourceCreationMode::COMMITED);
		m_testCubeInstanceDataBuffer.SetName(L"testCubeInstanceDataBufffer");
		//create local light data  buffer srv in the heap created
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC locallightbuffersrvdesc = {};
			locallightbuffersrvdesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
			locallightbuffersrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			locallightbuffersrvdesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			locallightbuffersrvdesc.Buffer.FirstElement = 0;
			locallightbuffersrvdesc.Buffer.NumElements = static_cast<UINT>(m_localLights.size());
			locallightbuffersrvdesc.Buffer.StructureByteStride = sizeof(PointLight);
			m_localLightsBuffer.CreateSRV(m_creationdevice, locallightbuffersrvdesc, m_SRVHeap.GetCPUHandleOffseted(1));

		}
		m_localLightdatabufferNeedUpdate = true;
		UpdateLocalLightBufferData();
		//create instance data  buffer srv in the heap created
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC instancebufferSRVdesc = {};
			instancebufferSRVdesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
			instancebufferSRVdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			instancebufferSRVdesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			instancebufferSRVdesc.Buffer.FirstElement = 0;
			instancebufferSRVdesc.Buffer.NumElements = NUMCUBESTORENDER;
			instancebufferSRVdesc.Buffer.StructureByteStride = sizeof(CubeInstanceData);
			m_testCubeInstanceDataBuffer.CreateSRV(m_creationdevice, instancebufferSRVdesc, m_SRVHeap.GetCPUHandleOffseted(2));

		}

	}
	BasicModelManager::LoadModel(m_creationdevice, "models/cube.dae", m_loadedmodel, VERTEXVERSION2);
#ifdef USECONFERENCEROOMCOMPOUNDMODEL
	float scalefactor = 0.01f;
	XMMATRIX scalemat = XMMatrixScalingFromVector(XMVectorSet(scalefactor, scalefactor, scalefactor, 1.0f));
	BasicModelManager::LoadModel(m_creationdevice, "models/conference.obj", m_loadedcompoundmodel, VERTEXVERSION2);
	m_loadedcompoundmodel.Extratransform(scalemat);
#endif // USECONFERENCEROOMCOMPOUNDMODEL


#if defined(USESPHONZAMODEL)
	float scalefactor = 1.0f;
	XMMATRIX scalemat = XMMatrixScalingFromVector(XMVectorSet(scalefactor, scalefactor, scalefactor, 1.0f));
	BasicModelManager::LoadModel(m_creationdevice, "models/Sponza.gltf", m_loadedcompoundmodel, VERTEXVERSION3, L"textures/modeltextures/sponza/", true);
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
	BasicModelManager::InitCubeModel(m_creationdevice, m_cubemodel_simpleTesting, VertexVersion::VERTEXVERSION3);
	Model::MaterialConstants cubeMaterialConstants = m_cubemodel_simpleTesting.GetMaterialConstants();
	cubeMaterialConstants.texsrvidx = 0;//arbatary unused for test cube model.
	m_cubemodel_simpleTesting.SetMaterialConstants(cubeMaterialConstants);
	//simple light representation cube
	BasicModelManager::InitCubeModel(m_creationdevice, m_cubemodel_simpleLight, VertexVersion::VERTEXVERSION3);
	Model::MaterialConstants SimplelightcubeMaterialConstants = m_cubemodel_simpleLight.GetMaterialConstants();
	SimplelightcubeMaterialConstants.texsrvidx = 0;//arbatary unused for simplelight cube model.
	m_cubemodel_simpleLight.SetMaterialConstants(SimplelightcubeMaterialConstants);
	InitPSO();

	m_planemodel.UploadModelDatatoBuffers();
	m_cubemodel_simpleTesting.UploadModelDatatoBuffers();
	m_loadedmodel.UploadModelDatatoBuffers();
	m_loadedcompoundmodel.UploadModelDatatoBuffers();
	m_cubemodel_simpleLight.UploadModelDatatoBuffers();

	m_uploadcommandlist.Reset();
	m_planemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_loadedmodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	//m_loadedcompoundmodel.UploadModelDataDefaultTexture(m_uploadcommandlist);
	m_loadedcompoundmodel.UploadData(m_uploadcommandlist);
	//m_loadedcompoundmodel.UploadModelTextureData(m_uploadcommandlist);
	m_cubemodel_simpleTesting.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_cubemodel_simpleLight.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	m_trianglemodel.UploadModelDatatoGPUBuffers(m_uploadcommandlist);
	DXASSERT(m_uploadcommandlist->Close());
	//upload instance data
	UpdateInstanceDataBuffer();
}

void LightingTestApplication::InitPSO()
{
	vector<D3D12_INPUT_ELEMENT_DESC> inputelements;
	DXVertexManager::BuildDefaultInputelementdesc(inputelements, VERTEXVERSION3);
	PSOInitData psoinitdata;
	psoinitdata.type = PSOType::GRAPHICS;

	DX12Shader* vs = new DX12Shader();
	DX12Shader* ps = new DX12Shader();
	vs->Init(L"shaders/lightingtest/LightingTestVertexShader.hlsl", DX12Shader::ShaderType::VS);
	ps->Init(L"shaders/lightingtest/LightingTestPixelShader.hlsl", DX12Shader::ShaderType::PS);
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
		{
			D3D12_ROOT_PARAMETER rootparam0 = BuildBasicCameraDataRootConstantParameterCommon();
			rootparams.push_back(rootparam0);
			//making mat table srv range
			D3D12_DESCRIPTOR_RANGE materialtablerange = {};
			materialtablerange.OffsetInDescriptorsFromTableStart = 0;
			materialtablerange.NumDescriptors = 1;
			materialtablerange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			materialtablerange.RegisterSpace = 1;
			materialtablerange.BaseShaderRegister = 0;
			//local light srv data range
			D3D12_DESCRIPTOR_RANGE locallightdatarange = {};
			locallightdatarange.OffsetInDescriptorsFromTableStart = 0;
			locallightdatarange.NumDescriptors = 1;
			locallightdatarange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			locallightdatarange.RegisterSpace = 0;
			locallightdatarange.BaseShaderRegister = 0;
			locallightdatarange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			//instance data srv data range
			D3D12_DESCRIPTOR_RANGE instancedatarange = {};
			instancedatarange.OffsetInDescriptorsFromTableStart = 0;
			instancedatarange.NumDescriptors = 1;
			instancedatarange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			instancedatarange.RegisterSpace = 1;
			instancedatarange.BaseShaderRegister = 1;
			instancedatarange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			//making unbound range so make sure it is last range
			D3D12_DESCRIPTOR_RANGE texturesrvrange = {};
			texturesrvrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			texturesrvrange.BaseShaderRegister = 1;
			texturesrvrange.RegisterSpace = 0;
			texturesrvrange.NumDescriptors = -1;
			texturesrvrange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			//order of range is important here else issue in unbound range(all texture).
			descrangestouserootparam1.push_back(materialtablerange);
			descrangestouserootparam1.push_back(locallightdatarange);
			descrangestouserootparam1.push_back(instancedatarange);
			descrangestouserootparam1.push_back(texturesrvrange);
			






			D3D12_ROOT_PARAMETER rootparam1 = {};
			rootparam1.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootparam1.DescriptorTable.NumDescriptorRanges = descrangestouserootparam1.size();
			rootparam1.DescriptorTable.pDescriptorRanges = descrangestouserootparam1.data();
			rootparam1.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
			rootparams.push_back(rootparam1);
			D3D12_ROOT_PARAMETER rootparam2 = {};
			rootparam2.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rootparam2.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootparam2.Constants.Num32BitValues = sizeof(Model::MaterialConstants) / 4;
			rootparam2.Constants.RegisterSpace = 1;
			rootparam2.Constants.ShaderRegister = 1;
			rootparams.push_back(rootparam2);
			D3D12_ROOT_PARAMETER rootparam3 = {};
			rootparam3.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rootparam3.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootparam3.Constants.Num32BitValues = sizeof(CustomMaterial) / 4;
			rootparam3.Constants.RegisterSpace = 0;
			rootparam3.Constants.ShaderRegister = 2;
			rootparams.push_back(rootparam3);
			D3D12_ROOT_PARAMETER rootparam4 = {};
			rootparam4.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			rootparam4.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootparam4.Constants.Num32BitValues = sizeof(TestLight) / 4;
			rootparam4.Constants.RegisterSpace = 0;
			rootparam4.Constants.ShaderRegister = 3;
			rootparams.push_back(rootparam4);


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
void LightingTestApplication::IMGUIRenderAdditional()
{
	//imgui mouse lock related
	{
		ImGui::Text("press 'x' key to toggle mouse camera move lock");
		char currentMouseMoveState[30];
		snprintf(currentMouseMoveState, 30, "mouse move camera allowed:%d", m_Imgui_mousecontrol_camera);
		ImGui::Text(currentMouseMoveState);
	}
	ImGui::Text("light properties:-");
	//test lighting source
	{
		ImGui::Checkbox("usedirectionallight only", &m_useDirectionalLighting);
	}
	//locallight related
	XMFLOAT3 tmppLightPos{ pointLightprimary.lightPos.x,pointLightprimary.lightPos.y,pointLightprimary.lightPos.z};
	ImGui::InputFloat3("point light_position", (float*)(&tmppLightPos));
	pointLightprimary.lightPos.x = tmppLightPos.x;
	pointLightprimary.lightPos.y = tmppLightPos.y;
	pointLightprimary.lightPos.z = tmppLightPos.z;
	//directional light related
	{
		ImGui::InputFloat3("directionallight_direction", (float*)(&m_TestLightProperties.lightDir));
	}
	//IMGUISimpleTestRender();
}
void LightingTestApplication::ProcessWindowProcEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	m_maincameracontroller.ProcessWindowProcEvent(hwnd, uMsg, wParam, lParam);
	if (uMsg == WM_CHAR)
	{
		char charpressed = (char)wParam;
		if (charpressed == 'x')
		{
			m_Imgui_mousecontrol_camera = !m_Imgui_mousecontrol_camera;
			m_maincameracontroller.AllowMouseMovementProcessing(m_Imgui_mousecontrol_camera);
		}
	}

}
void LightingTestApplication::UpdateLocalLightBufferData()
{
	if (m_localLightdatabufferNeedUpdate)
	{
		BufferMapParams lightBufferwriteparams = {};
		lightBufferwriteparams.range.Begin = 0;
		lightBufferwriteparams.range.End = m_localLightsBuffer.GetSize();
		void* lightbuffmapped = m_localLightsBuffer.Map(lightBufferwriteparams);
		memcpy(lightbuffmapped, m_localLights.data(), m_localLightsBuffer.GetSize());
		m_localLightsBuffer.UnMap(lightBufferwriteparams);
		m_localLightdatabufferNeedUpdate = false;
	}
}
void LightingTestApplication::UpdateInstanceDataBuffer()
{
	BufferMapParams instanceBufferwriteparams = {};
	instanceBufferwriteparams.range.Begin = 0;
	instanceBufferwriteparams.range.End = m_testCubeInstanceDataBuffer.GetSize();
	void* instanceDatabuffmapped = m_testCubeInstanceDataBuffer.Map(instanceBufferwriteparams);
	memcpy(instanceDatabuffmapped, m_instanceData.data(), m_testCubeInstanceDataBuffer.GetSize());
	m_testCubeInstanceDataBuffer.UnMap(instanceBufferwriteparams);
}
D3D12_ROOT_PARAMETER LightingTestApplication::BuildBasicCameraDataRootConstantParameterCommon()
{
	const bool initAsdescriptor = true;
	D3D12_ROOT_PARAMETER rootParamCamConst = {};
	//init as 32 bit const
	if (!initAsdescriptor)
	{
		rootParamCamConst.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		rootParamCamConst.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParamCamConst.Constants.Num32BitValues = sizeof(ShaderTransformConstants_GeneralComplete) / 4;
		rootParamCamConst.Constants.RegisterSpace = 0;
		rootParamCamConst.Constants.ShaderRegister = 0;
	}
	//init as cbv
	else
	{
		rootParamCamConst.Descriptor.ShaderRegister = 0;
		rootParamCamConst.Descriptor.RegisterSpace = 0;
	}

	return rootParamCamConst;
}