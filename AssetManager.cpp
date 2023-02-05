#include"AssetManager.h"
#include"DX12CommandList.h"





std::vector<Vertex>  cubeverticices = {
	{-1.0f, -1.0f, -1.0f,0.0,0.0f},	//0
	{-1.0f,  1.0f, -1.0f,0.0,0.0f},	//1
	{1.0f,  1.0f, -1.0f ,0.0,0.0f},	//2
	{1.0f, -1.0f, -1.0f,0.0,0.0f},	//3
	{-1.0f,-1.0f, 1.0f,0.0,0.0f},	//4
	{-1.0f, 1.0f, 1.0f,0.0,0.0f},	//5
	{ 1.0f, 1.0f, 1.0f,0.0,0.0f},	//6
	{ 1.0f,-1.0f, 1.0f,0.0,0.0f},	//7
};

/*std::vector<Vertex>  cubeverticices2 = {
	{-1.0f, -1.0f, -1.0f,0.0,0.0f,0.0f},	//0
	{-1.0f,  1.0f, -1.0f,0.0,1.0f,0.0f},	//1
	{1.0f,  1.0f, -1.0f ,1.0f,1.0f,0.0f},	//2
	{1.0f, -1.0f, -1.0f,1.0,0.0f,0.0f},	//3
	{-1.0f,-1.0f, 1.0f,0.0,0.0f,1.0f},	//4
	{-1.0f, 1.0f, 1.0f,0.0,1.0f,1.0f},	//5
	{ 1.0f, 1.0f, 1.0f,1.0,1.0f,1.0f},	//6
	{ 1.0f,-1.0f, 1.0f,1.0,0.0f,1.0f},	//7
};*/



std::vector<unsigned>cubeindicies =
{
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
};
std::vector<unsigned> planeindicies =
{
	0,1,2,
	0,2,3
};
std::vector<unsigned> triangleindicies =
{
	0,1,2
};
//repesentation of a single mesh vertex


Model::Model(ModelDataUploadMode uploadmode)
	:m_uploadmode(uploadmode),
	m_vertexversion(UNKNOWN)
{
	m_transform = XMMatrixIdentity();
}

Model::~Model()
{
	//verticies are dynamic allocations so delete
	for (size_t i = 0; i < m_verticies.size(); i++)
	{
		if (m_verticies[i])
		{
			delete m_verticies[i];
		}
	}
}
void Model::Draw(DX12Commandlist& renderingcmdlist,XMMATRIX vpmatrix, UINT mvpmatrixrootparamindex, bool usemodelmatrix, bool setmvpmatrix)
{
	renderingcmdlist->IASetVertexBuffers(0, 1, &m_vertexbufferview);
	renderingcmdlist->IASetIndexBuffer(&m_indexbufferview);
	XMMATRIX mvp = vpmatrix;
	if (usemodelmatrix)
	{
		mvp = XMMatrixMultiply(m_transform, vpmatrix);
	}
	
	if (setmvpmatrix)
	{
		renderingcmdlist->SetGraphicsRoot32BitConstants(mvpmatrixrootparamindex, sizeof(XMMATRIX) / 4, &mvp, 0);
	}
	
	renderingcmdlist->DrawIndexedInstanced(GetIndiciesCount(), 1, 0, 0, 0);
}
void Model::Extratransform(XMMATRIX extratransformmat)
{
	//m_transform = XMMatrixMultiply(extratransformmat, m_transform);
	m_transform = XMMatrixMultiply(m_transform,extratransformmat);
}

void Model::InitVertexBuffer(ComPtr< ID3D12Device> creationdevice, vector<VertexBase*>& verticies)
{

	m_verticies = verticies;
	BuildVertexRawData();
	DX12ResourceCreationProperties vbproperties;
	vbproperties.resheapflags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE;
	vbproperties.resinitialstate = D3D12_RESOURCE_STATE_COMMON;
	vbproperties.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	vbproperties.resheapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	vbproperties.resheapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	vbproperties.resheapprop.CreationNodeMask = 0;
	vbproperties.resheapprop.VisibleNodeMask = 0;
	vbproperties.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vbproperties.resdesc.Alignment = 0;
	vbproperties.resdesc.DepthOrArraySize = 1;
	vbproperties.resdesc.MipLevels = 1;
	vbproperties.resdesc.Height = 1;
	unsigned vertexsize = DXVertexManager::GetVertexSize(m_vertexversion);
	vbproperties.resdesc.Width = vertexsize *(UINT64)verticies.size();
	vbproperties.resdesc.Format = DXGI_FORMAT_UNKNOWN;
	vbproperties.resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vbproperties.resdesc.SampleDesc.Count = 1;
	vbproperties.resdesc.SampleDesc.Quality = 0;
	vbproperties.resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	
	

	//prepare vbview as well
	m_vertexuploadbuffer.Init(creationdevice, vbproperties, ResourceCreationMode::COMMITED);
	m_vertexuploadbuffer.SetName(L"planeVBupload");
	if (m_uploadmode == ModelDataUploadMode::COPY)
	{
		vbproperties.resheapprop.Type = D3D12_HEAP_TYPE_DEFAULT;
		m_vertexbuffer.Init(creationdevice, vbproperties, ResourceCreationMode::COMMITED);
		m_vertexbuffer.SetName(L"planeVB");
		m_vertexbufferview.BufferLocation = m_vertexbuffer.GetResource()->GetGPUVirtualAddress();
	}
	else
	{
		m_vertexbufferview.BufferLocation = m_vertexuploadbuffer.GetResource()->GetGPUVirtualAddress();
	}
	m_vertexbufferview.SizeInBytes = m_vertexuploadbuffer.GetSize();
	m_vertexbufferview.StrideInBytes=DXVertexManager::GetVertexSize(m_vertexversion);

	



}
void Model::InitIndexBuffer(ComPtr< ID3D12Device> creationdevice,vector<unsigned>& indicies)
{
	m_indicies = indicies;
	DX12ResourceCreationProperties ibproperties;
	ibproperties.resheapflags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE;
	ibproperties.resinitialstate = D3D12_RESOURCE_STATE_COMMON;
	ibproperties.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	ibproperties.resheapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	ibproperties.resheapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	ibproperties.resheapprop.CreationNodeMask = 0;
	ibproperties.resheapprop.VisibleNodeMask = 0;
	ibproperties.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ibproperties.resdesc.Alignment = 0;
	ibproperties.resdesc.DepthOrArraySize = 1;
	ibproperties.resdesc.MipLevels = 1;
	ibproperties.resdesc.Height = 1;
	ibproperties.resdesc.Width = sizeof(indicies[0]) * (UINT64)indicies.size();
	ibproperties.resdesc.Format = DXGI_FORMAT_UNKNOWN;
	ibproperties.resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ibproperties.resdesc.SampleDesc.Count = 1;
	ibproperties.resdesc.SampleDesc.Quality = 0;
	ibproperties.resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	m_indexuploadbuffer.Init(creationdevice, ibproperties, ResourceCreationMode::COMMITED);
	m_indexuploadbuffer.SetName(L"planeIBupload");
	ibproperties.resheapprop.Type = D3D12_HEAP_TYPE_DEFAULT;
	
	//prepare index buffer view
	if (m_uploadmode == ModelDataUploadMode::COPY)
	{
		m_indexbuffer.Init(creationdevice, ibproperties, ResourceCreationMode::COMMITED);
		m_indexbuffer.SetName(L"planeIB");
		m_indexbufferview.BufferLocation = m_indexbuffer.GetResource()->GetGPUVirtualAddress();
	}
	else
	{
		m_indexbufferview.BufferLocation = m_indexuploadbuffer.GetResource()->GetGPUVirtualAddress();
	}
	m_indexbufferview.SizeInBytes = m_indexuploadbuffer.GetSize();
	m_indexbufferview.Format = DXGI_FORMAT_R32_UINT;

	




}

void Model::Init(ComPtr< ID3D12Device> creationdevice, AssimpLoadedModel& assimpModel, UINT meshindexinassimpmodeltoload, VertexVersion modelvertexversion)
{
	SetVertexVersionUsed(modelvertexversion);
	vector<VertexBase*> verticies;
	AssimpLoadedMesh& meshtoload = assimpModel.m_meshes[meshindexinassimpmodeltoload];
	m_transform=AssimpManager::ToXMMatrix(meshtoload.transform);
	m_material = meshtoload.material;
	
	
	
	GetVertexArray(verticies,meshtoload, modelvertexversion);
	//currently taking in first mesh alone
	InitIndexBuffer(creationdevice, meshtoload.indicies);
	InitVertexBuffer(creationdevice, verticies);

}

D3D12_GPU_VIRTUAL_ADDRESS Model::GetVertexBufferGPUVirtualAddress()
{
	if (m_vertexbuffer.GetResource().Get())
	{
		return m_vertexbuffer.GetResource()->GetGPUVirtualAddress();
	}
	return NULL;
}
D3D12_GPU_VIRTUAL_ADDRESS Model::GetIndexBufferGPUVirtualAddress()
{
	if (m_indexbuffer.GetResource().Get())
	{
		return m_indexbuffer.GetResource()->GetGPUVirtualAddress();
	}
	return NULL;
}

void Model::GetVertexArray(vector<VertexBase*>& outverticies, AssimpLoadedMesh& ameshtoadd, VertexVersion vertversion)
{
	unsigned indexoffset = outverticies.size();
	for (size_t i = 0; i < ameshtoadd.verticies.size(); i++)
	{

		const AssimpLoadedVertex& aprocessedvertex = ameshtoadd.verticies[i];
		switch (vertversion)
		{
		case VERTEXVERSION0:
		{VetexV0* vert = new VetexV0();
		vert->m_position.x = aprocessedvertex.pos.x;
		vert->m_position.y = aprocessedvertex.pos.y;
		vert->m_position.z = aprocessedvertex.pos.z;
		vert->m_uv.x = aprocessedvertex.uv.x;
		vert->m_uv.y = aprocessedvertex.uv.y;
		outverticies.push_back(vert);
		break;
		}
		
		case VERTEXVERSION2:
		{VetexV2* vert = new VetexV2();
		vert->m_position.x = aprocessedvertex.pos.x;
		vert->m_position.y = aprocessedvertex.pos.y;
		vert->m_position.z = aprocessedvertex.pos.z;
		vert->m_normal.x = aprocessedvertex.normal.x;
		vert->m_normal.y = aprocessedvertex.normal.y;
		vert->m_normal.z = aprocessedvertex.normal.z;
		outverticies.push_back(vert);
		break;
		}
	}
	}
}


void Model::BuildVertexRawData()
{
	for (size_t i = 0; i < m_verticies.size(); i++)
	{
		DXVertexManager::RetriveRawVertexData(m_vertexdataraw, m_verticies[i]);
	}
}



void Model::UploadModelDatatoBuffers()
{
	//upload vertexdata
	{
		BufferMapParams vbmapparams = {};
	vbmapparams.subresource = 0;
	//not reading
	vbmapparams.range.Begin = 0;
	vbmapparams.range.End = 0;

	void* vbmapped = m_vertexuploadbuffer.Map(vbmapparams);
	memcpy(vbmapped, m_vertexdataraw.data(), m_vertexuploadbuffer.GetSize());
	//written to full range
	vbmapparams.range.End = m_vertexuploadbuffer.GetSize();
	m_vertexuploadbuffer.UnMap(vbmapparams);

	}

	//upload indexdata
	{
		BufferMapParams ibmapparams = {};
		ibmapparams.subresource = 0;
		//not reading
		ibmapparams.range.Begin = 0;
		ibmapparams.range.End = 0;

		void* ibmapped = m_indexuploadbuffer.Map(ibmapparams);
		memcpy(ibmapped, m_indicies.data(), m_indexuploadbuffer.GetSize());
		//written to full range
		ibmapparams.range.End = m_indexuploadbuffer.GetSize();
		m_indexuploadbuffer.UnMap(ibmapparams);

	}
}

void Model::UploadModelDatatoGPUBuffers(DX12Commandlist& copycmdlist)

{
	if (m_uploadmode != COPY)
	{
		return;
	}
	//copy vb
	//trnsition state for target
	D3D12_RESOURCE_BARRIER barrier = m_vertexbuffer.TransitionResState(D3D12_RESOURCE_STATE_COPY_DEST);
	copycmdlist->ResourceBarrier(1, &barrier);
	copycmdlist->CopyResource(m_vertexbuffer.GetResource().Get(), m_vertexuploadbuffer.GetResource().Get());
	barrier = m_vertexbuffer.TransitionResState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	copycmdlist->ResourceBarrier(1, &barrier);

	//copy ib
	barrier=m_indexbuffer.TransitionResState(D3D12_RESOURCE_STATE_COPY_DEST);
	copycmdlist->ResourceBarrier(1, &barrier);
	copycmdlist->CopyResource(m_indexbuffer.GetResource().Get(), m_indexuploadbuffer.GetResource().Get());
	barrier = m_indexbuffer.TransitionResState(D3D12_RESOURCE_STATE_INDEX_BUFFER);
	copycmdlist->ResourceBarrier(1, &barrier);

}

CompoundModel::CompoundModel(ModelDataUploadMode uploadmode)
	:m_datauploadmode(uploadmode)
{

}
CompoundModel::~CompoundModel()
{
	for (size_t i = 0; i < m_models.size(); i++)
	{
		delete m_models[i];
	}
}
void CompoundModel::Draw(DX12Commandlist& renderingcmdlist, XMMATRIX vpmatrix, UINT mvpmatrixrootparamindex)
{
	for (size_t i = 0; i < m_models.size(); i++)
	{
		m_models[i]->Draw(renderingcmdlist,vpmatrix,mvpmatrixrootparamindex);
	}
}
void CompoundModel::Extratransform(XMMATRIX extratransformmat)
{
	for (size_t i = 0; i < m_models.size(); i++)
	{
		m_models[i]->Extratransform(extratransformmat);
	}
}

void CompoundModel::UploadModelDatatoBuffers()
{
	for (size_t i = 0; i < m_models.size(); i++)
	{
		m_models[i]->UploadModelDatatoBuffers();
	}
}
void CompoundModel::UploadModelDatatoGPUBuffers(DX12Commandlist& copycmdlist)
{
	for (size_t i = 0; i < m_models.size(); i++)
	{
		m_models[i]->UploadModelDatatoGPUBuffers(copycmdlist);
	}
}

void CompoundModel::Init(ComPtr< ID3D12Device> creationdevice, AssimpLoadedModel& assimpModel, VertexVersion modelvertexversion)
{
	for (size_t i = 0; i < assimpModel.m_meshes.size(); i++)
	{
		Model* amodel = new Model(m_datauploadmode);
		amodel->Init(creationdevice, assimpModel, (UINT)i, modelvertexversion);
		AddModel(amodel);
	}
}







void BasicModelManager::InitPlaneModel(ComPtr< ID3D12Device> creationdevice, Model& planemodel)
{
	vector<VertexBase*> verticies;
	GetPlaneVerticiesV0(verticies);
	planemodel.SetVertexVersionUsed(VertexVersion::VERTEXVERSION0);
	planemodel.InitVertexBuffer(creationdevice, verticies);
	planemodel.InitIndexBuffer(creationdevice, planeindicies);
	planemodel.UploadModelDatatoBuffers();
}
void BasicModelManager::InitCubeModel(ComPtr< ID3D12Device> creationdevice, Model& cubemodel)
{
	vector<VertexBase*> verticies;
	GetCubeVerticiesV0(verticies);
	cubemodel.SetVertexVersionUsed(VERTEXVERSION0);
	cubemodel.InitVertexBuffer(creationdevice, verticies);
	cubemodel.InitIndexBuffer(creationdevice,cubeindicies);
	cubemodel.UploadModelDatatoBuffers();
}
void BasicModelManager::InitTriangleModel(ComPtr< ID3D12Device> creationdevice, Model& trianglemodel)
{
	
	vector<VertexBase*> verticies;
	GetTriangleVerticiesV0(verticies);
	trianglemodel.SetVertexVersionUsed(VERTEXVERSION0);
	trianglemodel.InitVertexBuffer(creationdevice, verticies);
	trianglemodel.InitIndexBuffer(creationdevice, triangleindicies);
	trianglemodel.UploadModelDatatoBuffers();
}

void BasicModelManager::LoadModel(ComPtr< ID3D12Device> creationdevice,std::string modelfilepath, Model& outmodel,VertexVersion requiredvertexversion)
{
	AssimpManager assimpmodel(modelfilepath);
	outmodel.Init(creationdevice,assimpmodel.GetProcessedModel(),0, requiredvertexversion);
}
void BasicModelManager::LoadModel(ComPtr< ID3D12Device> creationdevice, std::string modelfilepath, CompoundModel& outmodel, VertexVersion requiredvertexversion)
{
	AssimpManager assimpmodel(modelfilepath);
	outmodel.Init(creationdevice, assimpmodel.GetProcessedModel(), requiredvertexversion);
}

void BasicModelManager::GetPlaneVerticiesV0(vector<VertexBase*>& outverticies)
{
	//0(left-top)
	VetexV0* vert = new VetexV0();
	vert->m_position = { -1.0f, 1.0f, 0.0f };
	vert->m_uv = { 0.0f,0.0f };
	outverticies.push_back(vert);
	//1(left-bottom)
	vert = new VetexV0();
	vert->m_position = { -1.0f,-1.0f,0.0f };
	vert->m_uv = { 0.0f,1.0f };
	outverticies.push_back(vert);
	//2(right-bottom)
	vert = new VetexV0();
	vert->m_position = { 1.0f,-1.0f,0.0f };
	vert->m_uv = { 1.0f,1.0f };
	outverticies.push_back(vert);
	//3(right-top)
	vert = new VetexV0();
	vert->m_position = { 1.0f,1.0f,0.0f };
	vert->m_uv = { 1.0f,0.0f };
	outverticies.push_back(vert);
}

void BasicModelManager::GetCubeVerticiesV0(vector<VertexBase*>& outverticies)
{
	

	//0
	VetexV0* vert = new VetexV0();
	vert->m_position = { -1.0f, -1.0f, -1.0f };
	vert->m_uv = { 0.0,0.0f };
	outverticies.push_back(vert);
	//1
	 vert = new VetexV0();
	vert->m_position = { -1.0f,  1.0f, -1.0f };
	vert->m_uv = { 0.0,0.0f };
	outverticies.push_back(vert);
	//2
	 vert = new VetexV0();
	vert->m_position = { 1.0f,  1.0f, -1.0f };
	vert->m_uv = { 0.0,0.0f };
	outverticies.push_back(vert);
	//3
	 vert = new VetexV0();
	vert->m_position = { 1.0f, -1.0f, -1.0f };
	vert->m_uv = { 0.0,0.0f };
	outverticies.push_back(vert);
	//4
	 vert = new VetexV0();
	vert->m_position = { -1.0f,-1.0f, 1.0f };
	vert->m_uv = { 0.0,0.0f };
	outverticies.push_back(vert);
	//5
	 vert = new VetexV0();
	vert->m_position = { -1.0f, 1.0f, 1.0f };
	vert->m_uv = { 0.0,0.0f };
	outverticies.push_back(vert);
	//6
	vert = new VetexV0();
	vert->m_position = { 1.0f, 1.0f, 1.0f };
	vert->m_uv = { 0.0,0.0f };
	outverticies.push_back(vert);
	//7
	vert = new VetexV0();
	vert->m_position = { 1.0f,-1.0f, 1.0f };
	vert->m_uv = { 0.0,0.0f };
	outverticies.push_back(vert);
}
void BasicModelManager::GetTriangleVerticiesV0(vector<VertexBase*>& outverticies)
{
	//0(center-top)
	VetexV0* vert = new VetexV0();
	vert->m_position = { 0.0f, 1.0f, 0.0f };
	vert->m_uv = { 0.5f,1.0f };
	outverticies.push_back(vert);
	//1(left-bottom)
	vert = new VetexV0();
	vert->m_position = { -1.0f,-1.0f,0.0f };
	vert->m_uv = { 0.0f,0.0f };
	outverticies.push_back(vert);
	//2(right-bottom)
	vert = new VetexV0();
	vert->m_position = { 1.0f,-1.0f,0.0f };
	vert->m_uv = { 1.0f,0.0f };
	outverticies.push_back(vert);
	
}
void BasicModelManager::GetTriangleRTVertexData(vector<RTVertexDataV0>& rtvertexdata)
{
	vector<VertexBase*> verticies;
	GetTriangleVerticiesV0(verticies);
	for (VertexBase* vertex : verticies)
	{
		VetexV0* vertexv0 = dynamic_cast<VetexV0*>(vertex);
		RTVertexDataV0 rtvert = {};
		rtvert.m_position = vertexv0->m_position;
		rtvert.m_uv = vertexv0->m_uv;
		rtvertexdata.push_back(rtvert);
		delete vertex;
	}
}