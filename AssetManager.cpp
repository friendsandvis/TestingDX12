#include"AssetManager.h"
#include"DX12CommandList.h"
#define ALLOWRENDERINGNONOPAQUEMODELS true





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
std::vector<VetexV0> planeVertexData_versionV0 =
{
	VetexV0({-1.0f, 1.0f, 0.0f}, {0.0f,0.0f}),//0
	VetexV0({-1.0f,-1.0f,0.0f}, {0.0f,1.0f}),//1
	VetexV0({1.0f,-1.0f,0.0f},  {1.0f,1.0f}),//2
	VetexV0({1.0f,1.0f,0.0f},   {1.0f,0.0f}),//3

};
//repesentation of a single mesh vertex


Model::Model(ModelDataUploadMode uploadmode)
	:m_uploadmode(uploadmode),
	m_vertexversion(UNKNOWN),
	m_tmpmaterialgpuindex(-1),
	m_Allowrender(true)
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
void Model::Draw(DX12Commandlist& renderingcmdlist,XMMATRIX vpmatrix, UINT mvpmatrixrootparamindex, UINT materialconstsrootparamindex, bool usemodelmatrix, bool setmvpmatrix, bool supportmaterial)
{
	if (!m_Allowrender)
	{
		return;
	}
	renderingcmdlist->IASetVertexBuffers(0, 1, &m_vertexbufferview);
	renderingcmdlist->IASetIndexBuffer(&m_indexbufferview);
	XMMATRIX mvp = vpmatrix;
	if (usemodelmatrix)
	{
		mvp = XMMatrixMultiply(m_transform, vpmatrix);
	}
	m_shadertransformconsts.mvp = mvp;
	m_shadertransformconsts.model = m_transform;
	
	if (setmvpmatrix)
	{
		renderingcmdlist->SetGraphicsRoot32BitConstants(mvpmatrixrootparamindex, sizeof(m_shadertransformconsts)/4, &m_shadertransformconsts, 0);

	}
	if (supportmaterial)
	{
		renderingcmdlist->SetGraphicsRoot32BitConstants(materialconstsrootparamindex, sizeof(m_matconsts) / 4, &m_matconsts, 0);
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

void Model::Init(ComPtr< ID3D12Device> creationdevice, AssimpLoadedModel& assimpModel, UINT meshindexinassimpmodeltoload, VertexVersion modelvertexversion, bool supportmaterial)
{
	SetVertexVersionUsed(modelvertexversion);
	vector<VertexBase*> verticies;
	AssimpLoadedMesh& meshtoload = assimpModel.m_meshes[meshindexinassimpmodeltoload];
	m_transform=AssimpManager::ToXMMatrix(meshtoload.transform);
	if (supportmaterial)
	{
		m_material = meshtoload.material;
		InitMaterial(creationdevice,m_texfilepath);
		bool hasTransparentMaterial = HasTransparentMaterial(m_texfilepath);
		//if model has a transparent material avoid rendering it for now.
		if (hasTransparentMaterial)
		{
			m_hasOpaqueMaterial = false;
			if (!ALLOWRENDERINGNONOPAQUEMODELS)
			{
				m_Allowrender = false;
			}
		}
	}
	
	
	
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
		case VERTEXVERSION3:
		{
			VertexV3* vert = new VertexV3();
			vert->m_position.x = aprocessedvertex.pos.x;
			vert->m_position.y = aprocessedvertex.pos.y;
			vert->m_position.z = aprocessedvertex.pos.z;
			vert->m_normal.x = aprocessedvertex.normal.x;
			vert->m_normal.y = aprocessedvertex.normal.y;
			vert->m_normal.z = aprocessedvertex.normal.z;
			vert->m_uv.x = aprocessedvertex.uv.x;
			vert->m_uv.y = aprocessedvertex.uv.y;
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
void Model::UploadModelTextureData(DX12Commandlist& copycmdlist)
{
	m_loadedmaterial.UploadTextures(copycmdlist);
}
void Model::TransitionVextexAndIndexBufferState(D3D12_RESOURCE_STATES state, ComPtr<ID3D12GraphicsCommandList4>cmdlist)
{
	//transition vertexbuffer to state
	D3D12_RESOURCE_BARRIER barrier=m_vertexbuffer.TransitionResState(state);
	if (DXUtils::IsBarrierSafeToExecute(barrier))
	{
		cmdlist->ResourceBarrier(1, &barrier);
	}
	//transition indexbuffer to state
	barrier = m_indexbuffer.TransitionResState(state);
	if (DXUtils::IsBarrierSafeToExecute(barrier))
	{
		cmdlist->ResourceBarrier(1, &barrier);
	}
}
void Model::InitMaterial(ComPtr< ID3D12Device> creationdevice, wstring texfilepath)
{
	m_loadedmaterial.SetTexPath(texfilepath);
	//diffuse texture load
	{
	std::set<std::string>& texnames = m_material.GetDiffuseTextureNames();
	if (texnames.size() > 0)
	{

		std::string texfilename = *(texnames.begin());
		wstring texfilenamewstr(texfilename.begin(), texfilename.end());


		m_loadedmaterial.LoadDifuseTexture(texfilenamewstr);
	}
	}
	//normal texture load
	{
		std::set<std::string>& texnames = m_material.GetNormalTextureNames();
		if (texnames.size() > 0)
		{
			std::string texfilename = *(texnames.begin());
			wstring texfilenamewstr(texfilename.begin(), texfilename.end());


			m_loadedmaterial.LoadNormalTexture(texfilenamewstr);
		}
	}
	//roughness texture load
	{
		std::set<std::string>& texnames = m_material.GetRoughnessTextureNames();
		if (texnames.size() > 0)
		{
			std::string texfilename = *(texnames.begin());
			wstring texfilenamewstr(texfilename.begin(), texfilename.end());


			m_loadedmaterial.LoadRoughnessTexture(texfilenamewstr);
		}
	}
	//metalness texture load
	{
		std::set<std::string>& texnames = m_material.GetMetalnessTextureNames();
		if (texnames.size() > 0)
		{
			std::string texfilename = *(texnames.begin());
			wstring texfilenamewstr(texfilename.begin(), texfilename.end());


			m_loadedmaterial.LoadMetalnessTexture(texfilenamewstr);
		}
	}
		m_loadedmaterial.Init(creationdevice);
}
void Model::GetMaterialTextures(vector< DXTexture*>& textures)
{
	m_loadedmaterial.GetMaterialTextures(textures);
}
bool Model::HasTransparentMaterial(wstring texfilepath)
{
	//m_loadedmaterial.SetTexPath(texfilepath);
	//diffuse texture check for any being transparent
	{
		std::set<std::string>& texnames = m_material.GetDiffuseTextureNames();
		if (texnames.size() > 0)
		{

			std::string texfilename = *(texnames.begin());
			wstring texfilenamewstr(texfilename.begin(), texfilename.end());
			wstring texpath = texfilepath + texfilenamewstr;
			if (DXTexManager::IsTextureTransparent(texpath.c_str()))
			{
				return true;
			}
		}
	}
	return false;
}
CompoundModel::CompoundModel(ModelDataUploadMode uploadmode)
	:m_datauploadmode(uploadmode),
	m_supportmaterial(false)
{
	m_currenttexidxtoupload = 0;
}
CompoundModel::~CompoundModel()
{
	for (size_t i = 0; i < m_models.size(); i++)
	{
		delete m_models[i];
	}
}
void CompoundModel::Draw(DX12Commandlist& renderingcmdlist, XMMATRIX vpmatrix, UINT mvpmatrixrootparamindex,UINT materialconstsrootparamindex)
{
	for (size_t i = 0; i < m_models.size(); i++)
	{
		m_models[i]->Draw(renderingcmdlist,vpmatrix,mvpmatrixrootparamindex,materialconstsrootparamindex,true,true,m_supportmaterial);
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
void CompoundModel::UploadModelDataDefaultTexture(DX12Commandlist& copycmdlist)
{
	D3D12_RESOURCE_BARRIER abarrier = m_whitetexuploadbuffer.TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE);
	if (DXUtils::IsBarrierSafeToExecute(abarrier))
	{
		copycmdlist->ResourceBarrier(1, &abarrier);
	}
	abarrier = m_whitetexture.TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);
	if (DXUtils::IsBarrierSafeToExecute(abarrier))
	{
		copycmdlist->ResourceBarrier(1, &abarrier);
	}
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = m_whitetexture.GetResource().Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = m_whitetexuploadbuffer.GetResource().Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	src.PlacedFootprint.Footprint.Width = 1;
	src.PlacedFootprint.Footprint.Height = 1;
	src.PlacedFootprint.Footprint.Depth = 1;
	src.PlacedFootprint.Footprint.RowPitch = m_whitetexsubresfootprint.Footprint.RowPitch;
	src.PlacedFootprint.Offset = 0;
	copycmdlist->CopyTextureRegion(&dst, 0, 0, 0, &src, NULL);
	abarrier = m_whitetexture.TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	if (DXUtils::IsBarrierSafeToExecute(abarrier))
	{
		copycmdlist->ResourceBarrier(1, &abarrier);
	}

}
void CompoundModel::UploadData(DX12Commandlist& copycmdlist, bool uploaddefaults, bool uploadmodeldatatogpubuffers)
{
	/*if (uploaddefaults)
	{
		UploadModelDataDefaultTexture(copycmdlist);
	}*/
	if (uploadmodeldatatogpubuffers)
	{
		UploadModelDatatoGPUBuffers(copycmdlist);
	}
	
}
void CompoundModel::UploadModelTextureData(DX12Commandlist& copycmdlist)
{
	assert(m_supportmaterial);
	/*for (size_t i = 0; i < m_models.size(); i++)
	{
		m_models[i]->UploadModelTextureData(copycmdlist);
	}*/
	for (DXTexture* tex : m_texturestoupload)
	{
		tex->UploadTexture(copycmdlist);
	}
}
void CompoundModel::UploadCurrentFrameModelTextureData(DX12Commandlist& copycmdlist, bool increment)
{
	if (!m_supportmaterial)
	{
		return;
	}
	if (!NeedToUploadTextures())
	{
		return;
	}
	m_texturestoupload[m_currenttexidxtoupload]->UploadTexture(copycmdlist);
	if (increment)
	{
		m_currenttexidxtoupload++;
	}
}

void CompoundModel::Init(ComPtr< ID3D12Device> creationdevice, AssimpLoadedModel& assimpModel, VertexVersion modelvertexversion, bool supportmaterial)
{
	m_supportmaterial = supportmaterial;
	//init models and gather information required to build up the texture table(diffuse textures only for now)
	unsigned int numsrvrequired = 0;
	for (size_t i = 0; i < assimpModel.m_meshes.size(); i++)
	{
		Model* amodel = new Model(m_datauploadmode);
		amodel->SetTexPath(m_texfilepath);
		amodel->Init(creationdevice, assimpModel, (UINT)i, modelvertexversion,m_supportmaterial);
		if (m_supportmaterial)
		{
			//gather textures to upload
			amodel->GetMaterialTextures(m_texturestoupload);
			//gather number of textures for which srvs are to be created.
			//diffuse
			if (amodel->GetLoadedMaterial().GetDiffuseTexture())
			{
				numsrvrequired += 1;
			}
			//normal
			if (amodel->GetLoadedMaterial().GetNormalTexture())
			{
				numsrvrequired += 1;
			}
			//roughness/metalness(note we expect roughness/metalness to be packed in single texture.
			if (amodel->GetLoadedMaterial().GetRoughnessTexture())
			{
				numsrvrequired += 1;
			}
		}

		AddModel(amodel);
	}

	//collect data to build up texture srv heap
	
	
	UINT actualSRVtoallocate =numsrvrequired+1;//1 extra needed for material table.
	//prevent crash if no srv required(we do not need the heap then).
	if (actualSRVtoallocate)
	{
		D3D12_DESCRIPTOR_HEAP_DESC texsrvheapdesc = {};
		texsrvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		texsrvheapdesc.NumDescriptors = actualSRVtoallocate;
		texsrvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_resourceviewheap.Init(texsrvheapdesc, creationdevice);
		//create actual srvs
		int indexinsrvheaptouse = 1;//0th place used for material table buffer
		for (size_t i = 0; i < m_models.size(); i++)
		{
				//handle diffuse texture
				DXTexture* diffusetex = m_models[i]->GetLoadedMaterial().GetDiffuseTexture();
				if (diffusetex)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
					srvdesc.Texture2D.MipLevels = diffusetex->GetTotalMipCount();
					srvdesc.Format = diffusetex->GetDXImageData().m_imagemetadata.format;
					srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					diffusetex->CreateSRV(creationdevice, srvdesc, m_resourceviewheap.GetCPUHandleOffseted(indexinsrvheaptouse));

					m_models[i]->SetDiffusetextureIdx(indexinsrvheaptouse);
					indexinsrvheaptouse++;

				}
				//handle normal texture
				DXTexture* normaltex = m_models[i]->GetLoadedMaterial().GetNormalTexture();
				if (normaltex)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
					srvdesc.Texture2D.MipLevels = normaltex->GetTotalMipCount();
					srvdesc.Format = normaltex->GetDXImageData().m_imagemetadata.format;
					srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					normaltex->CreateSRV(creationdevice, srvdesc, m_resourceviewheap.GetCPUHandleOffseted(indexinsrvheaptouse));
					m_models[i]->SetNormaltextureIdx(indexinsrvheaptouse);
					indexinsrvheaptouse++;

				}
				//handle roughness/metalness texture
				DXTexture* roughnesstex = m_models[i]->GetLoadedMaterial().GetRoughnessTexture();
				if (roughnesstex)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
					srvdesc.Texture2D.MipLevels = roughnesstex->GetTotalMipCount();
					srvdesc.Format = roughnesstex->GetDXImageData().m_imagemetadata.format;
					srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					roughnesstex->CreateSRV(creationdevice, srvdesc, m_resourceviewheap.GetCPUHandleOffseted(indexinsrvheaptouse));
					m_models[i]->SetRoughnesstextureIdx(indexinsrvheaptouse);
					indexinsrvheaptouse++;

				}
				//now we have full material gpu dataready for model so retrive the material gpu data
				{
					m_allmaterialsused.push_back(m_models[i]->GetMaterialDataGPU());
					unsigned matidx = static_cast<unsigned>(m_allmaterialsused.size()) - 1;
					m_models[i]->SetMatGPUIdx(matidx);
				}

		}

	}
	if (m_supportmaterial)
	{
		//create materialtable buffer
		{
			DX12ResourceCreationProperties materialtablebufferprops;
			DX12Buffer::InitResourceCreationProperties(materialtablebufferprops);
			materialtablebufferprops.resdesc.Width = sizeof(MaterialDataGPU) * m_allmaterialsused.size();
			materialtablebufferprops.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
			m_materialtable.Init(creationdevice, materialtablebufferprops, ResourceCreationMode::COMMITED);
			m_materialtable.SetName(L"materialtablebuffer");
			//create material table buffer view
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC materialtablebuffersrvdesc = {};
				materialtablebuffersrvdesc.ViewDimension= D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
				materialtablebuffersrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				materialtablebuffersrvdesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
				materialtablebuffersrvdesc.Buffer.FirstElement = 0;
				materialtablebuffersrvdesc.Buffer.NumElements = static_cast<UINT>(m_allmaterialsused.size());
				materialtablebuffersrvdesc.Buffer.StructureByteStride = sizeof(MaterialDataGPU);
				m_materialtable.CreateSRV(creationdevice,materialtablebuffersrvdesc, m_resourceviewheap.GetCPUHandleOffseted(0));
			}
			//write materialtable
			BufferMapParams mattablewriteparams = {};
			mattablewriteparams.range.Begin = 0;
			mattablewriteparams.range.End = m_materialtable.GetSize();
			void* mattablebuffmapped = m_materialtable.Map(mattablewriteparams);
			memcpy(mattablebuffmapped, m_allmaterialsused.data(), m_materialtable.GetSize());
			m_materialtable.UnMap(mattablewriteparams);
		}
		//create whitetexture(default texture)
		 {
			DX12ResourceCreationProperties	whitetexresprops;
			DX12TextureSimple::InitResourceCreationProperties(whitetexresprops);
			whitetexresprops.resdesc.Width = whitetexresprops.resdesc.Height = whitetexresprops.resdesc.DepthOrArraySize = 1;
			whitetexresprops.resdesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			whitetexresprops.resdesc.MipLevels = 1;
			m_whitetexture.Init(creationdevice, whitetexresprops, ResourceCreationMode::COMMITED);
			m_whitetexture.SetName(L"whitetexture");

			UINT numrows;
			UINT64 totalbytes, rowsizebytes;
			creationdevice->GetCopyableFootprints(&whitetexresprops.resdesc, 0, 1, 0, &m_whitetexsubresfootprint, &numrows, &rowsizebytes, &totalbytes);
		}
		//prepare upload buffer with data for whitetex.
		{
			//created whitetexture is hardcoded in properties so we know it's size rrequirnments.
			DX12ResourceCreationProperties whitetexuploadbufferprops;
			DX12Buffer::InitResourceCreationProperties(whitetexuploadbufferprops);
			whitetexuploadbufferprops.resdesc.Width = 4;
			whitetexuploadbufferprops.resheapprop.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
			m_whitetexuploadbuffer.Init(creationdevice, whitetexuploadbufferprops, ResourceCreationMode::COMMITED);

		}
		{
			unsigned char whitetexdata[4] = { 255,255,255,255 };
			BufferMapParams mapparams;
			mapparams.range.Begin = 0;
			mapparams.range.End = m_whitetexuploadbuffer.GetSize();
			mapparams.subresource = 0;
			void* mappedbuffer = m_whitetexuploadbuffer.Map(mapparams);
			memcpy(mappedbuffer, whitetexdata, mapparams.range.End);
			BufferMapParams unmapparams = mapparams;
			m_whitetexuploadbuffer.UnMap(unmapparams);

		}
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC whitetexsrvdesc = {};
			whitetexsrvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			whitetexsrvdesc.Texture2D.MipLevels = 1;
			whitetexsrvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			//m_whitetexture.CreateSRV(creationdevice, whitetexsrvdesc, m_resourceviewheap.GetCPUHandleOffseted(1));
		}
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
void BasicModelManager::LoadModel(ComPtr< ID3D12Device> creationdevice, std::string modelfilepath, CompoundModel& outmodel, VertexVersion requiredvertexversion, wstring texfilepath, bool supportmaterial)
{
	AssimpManager assimpmodel(modelfilepath);
	outmodel.SetTexPath(texfilepath);
	outmodel.Init(creationdevice, assimpmodel.GetProcessedModel(), requiredvertexversion,supportmaterial);
}

void BasicModelManager::GetPlaneVerticiesV0(vector<VertexBase*>& outverticies)
{
	/*
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
	*/
	//use verticies from plane verticies vector
	for (const VetexV0& v : planeVertexData_versionV0)
	{
		VetexV0* vert = new VetexV0();
		(*vert) = v;
		outverticies.push_back(vert);
	}

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

ModelMaterial::ModelMaterial()
	:
	m_diffusetexture(nullptr),
	m_normaltexture(nullptr),
	m_roughnesstexture(nullptr),
	m_metalnesstexture(nullptr)
{

}
ModelMaterial::~ModelMaterial()
{
	if (m_diffusetexture)
	{
		delete m_diffusetexture;
	}
	if (m_normaltexture)
	{
		delete m_normaltexture;
	}
	if (m_roughnesstexture)
	{
		delete m_roughnesstexture;
	}
	if (m_metalnesstexture)
	{
		delete m_metalnesstexture;
	}
}
void ModelMaterial::LoadDifuseTexture(std::wstring texname)
{
	
	m_diffusetexture = new DXTexture();
	wstring texpath=GetTextureFilePath(texname);
	DXTexManager::LoadTexture(texpath.c_str(), m_diffusetexture->GetDXImageData());
}
void ModelMaterial::LoadNormalTexture(std::wstring texname)
{
	
	m_normaltexture = new DXTexture();
	wstring texpath = GetTextureFilePath(texname);
	DXTexManager::LoadTexture(texpath.c_str(), m_normaltexture->GetDXImageData());
}
void ModelMaterial::LoadRoughnessTexture(std::wstring texname)
{

	m_roughnesstexture = new DXTexture();
	wstring texpath = GetTextureFilePath(texname);
	DXTexManager::LoadTexture(texpath.c_str(), m_roughnesstexture->GetDXImageData());
}
void ModelMaterial::LoadMetalnessTexture(std::wstring texname)
{

	m_metalnesstexture = new DXTexture();
	wstring texpath = GetTextureFilePath(texname);
	DXTexManager::LoadTexture(texpath.c_str(), m_metalnesstexture->GetDXImageData());
}
std::wstring ModelMaterial::GetTextureFilePath(std::wstring texname)
{
	wstring texfilepath = m_texfilepath + texname;
	return texfilepath;
}
void ModelMaterial::UploadTextures(DX12Commandlist& copycmdlist)
{
	if (m_diffusetexture)
	{
		m_diffusetexture->UploadTexture(copycmdlist);
	}
	if (m_normaltexture)
	{
		m_normaltexture->UploadTexture(copycmdlist);
	}
	if (m_roughnesstexture)
	{
		m_roughnesstexture->UploadTexture(copycmdlist);
	}
	if (m_metalnesstexture)
	{
		m_metalnesstexture->UploadTexture(copycmdlist);
	}
}
void ModelMaterial::Init(ComPtr< ID3D12Device> creationdevice)
{
	if (m_diffusetexture)
	{
		m_diffusetexture->Init(creationdevice);
		
		m_diffusetexture->SetName(L"diffusetexture_modelmaterial");
	}
	if (m_normaltexture)
	{
		m_normaltexture->Init(creationdevice);
		m_diffusetexture->SetName(L"normaltexture_modelmaterial");
	}
	if (m_roughnesstexture)
	{
		m_roughnesstexture->Init(creationdevice);
		m_roughnesstexture->SetName(L"roughnesstexture_modelmaterial");
	}
	if (m_metalnesstexture)
	{
		m_metalnesstexture->Init(creationdevice);
		m_metalnesstexture->SetName(L"metalnesstexture_modelmaterial");
	}
}
void ModelMaterial::GetMaterialTextures(vector< DXTexture*>& textures)
{
	if (m_diffusetexture)
	{
		textures.push_back(m_diffusetexture);
	}
	if (m_normaltexture)
	{
		textures.push_back(m_normaltexture);
	}
	if (m_roughnesstexture)
	{
		textures.push_back(m_roughnesstexture);
	}
	if (m_metalnesstexture)
	{
		textures.push_back(m_metalnesstexture);
	}
}