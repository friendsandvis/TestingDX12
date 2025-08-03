#include"AssetManager.h"
#include"DX12CommandList.h"
//during model loading this macro is used to determine if we need to set rendering for non opaque models to allow rendering them or not.
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
float cubeverticicesV3[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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
	if (HasIndexBuffer())
	{
		renderingcmdlist->IASetIndexBuffer(&m_indexbufferview);
		renderingcmdlist->DrawIndexedInstanced(GetIndiciesCount(), 1, 0, 0, 0);
	}
	else
	{
		renderingcmdlist->DrawInstanced(m_verticies.size(), 1, 0, 0);
	}
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
	if(HasIndexBuffer())
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
	if (HasIndexBuffer())
	{
		barrier = m_indexbuffer.TransitionResState(D3D12_RESOURCE_STATE_COPY_DEST);
		copycmdlist->ResourceBarrier(1, &barrier);
		copycmdlist->CopyResource(m_indexbuffer.GetResource().Get(), m_indexuploadbuffer.GetResource().Get());
		barrier = m_indexbuffer.TransitionResState(D3D12_RESOURCE_STATE_INDEX_BUFFER);
		copycmdlist->ResourceBarrier(1, &barrier);
	}

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

			//if metalness texture same as roughness then probably they are in combined texture don't load metalness again.
			DXTexture* roughnesstexture=m_loadedmaterial.GetRoughnessTexture();
			bool hasroughnessmetalnesscombinedtexture = false;
			if (roughnesstexture)
			{
				hasroughnessmetalnesscombinedtexture = (roughnesstexture->GetExternalTextureFileName() == texfilenamewstr);
			}
			m_loadedmaterial.ContainCombinedMetalnessRoughnessTex(hasroughnessmetalnesscombinedtexture);
			if (!hasroughnessmetalnesscombinedtexture)
			{
				m_loadedmaterial.LoadMetalnessTexture(texfilenamewstr);
			}
		}
	}
		m_loadedmaterial.Init(creationdevice,false);
}
void Model::GetMaterialTextures(vector< DXTexture*>& textures)
{
	m_loadedmaterial.GetMaterialTextures(textures);
}
void Model::GetMaterialTexturesUploadInfo(vector< ModelMaterial::TextureUploadInfo>& textureUploadInfos)
{
	m_loadedmaterial.GetMaterialTexturesUploadInfo(textureUploadInfos);
}
bool Model::HasTransparentMaterial(wstring texfilepath)
{
	//during assimp model loading itself it can be has been determined if material supports transparency.
	return m_material.IsTransparent();
}
void Model::SetTransformation(DirectX::XMFLOAT4 scale, DirectX::XMFLOAT4 rotationaxis, float rotationangle, DirectX::XMFLOAT4 translate)
{
	m_transform = DXUtils::GetTransformationMatrix(DXUtils::Float4tovector(scale), DXUtils::Float4tovector(rotationaxis), rotationangle, DXUtils::Float4tovector(translate));
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
void CompoundModel::Draw(DX12Commandlist& renderingcmdlist, XMMATRIX vpmatrix, UINT mvpmatrixrootparamindex,UINT materialconstsrootparamindex, bool drawOpaque, bool drawNonOpaque)
{
	vector<Model*> opaqueModels, nonOpaqueModels;
	for (size_t i = 0; i < m_models.size(); i++)
	{
		//if not supporting nonopaque materials for this compound model then all models are sent to opaque node list
		if (m_models[i]->HasOpaqueMaterial() && m_supportNonOpaqueMaterial)
		{
			opaqueModels.push_back(m_models[i]);
		}
		else
		{
			nonOpaqueModels.push_back(m_models[i]);
		}
	}
	//draw opaque models
	if (drawOpaque)
	{
		for (size_t i = 0; i < opaqueModels.size(); i++)
		{
			opaqueModels[i]->Draw(renderingcmdlist, vpmatrix, mvpmatrixrootparamindex, materialconstsrootparamindex, true, true, m_supportmaterial);
		}
	}
		//draw nonopaque models
	if(drawNonOpaque)
	{
		for (size_t i = 0; i < nonOpaqueModels.size(); i++)
		{
			nonOpaqueModels[i]->Draw(renderingcmdlist, vpmatrix, mvpmatrixrootparamindex, materialconstsrootparamindex, true, true, m_supportmaterial);
		}
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
void CompoundModel::UploadCurrentFrameModelTextureData(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& copycmdlist, bool increment)
{
	if (!m_supportmaterial)
	{
		return;
	}
	if (!NeedToUploadTextureInfos())
	{
		return;
	}
	UploadModelTextureData(m_textureuploadInfos[m_currenttexidxtoupload],creationdevice,copycmdlist);
	if (increment)
	{
		m_currenttexidxtoupload++;
	}
}
void CompoundModel::UploadAllModelTextureData(DX12Commandlist& copycmdlist)
{
	if (!NeedToUploadTextures())
	{
		return;
	}
	//upload all textures's data sequentially
	for (size_t i = 0; i < m_texturestoupload.size(); i++)
	{
		m_texturestoupload[i]->UploadTexture(copycmdlist);
	}
	//update currenttexidxtoupload variable such that internal function can indicate no textures need upload. 
	m_currenttexidxtoupload = m_texturestoupload.size();
}
void CompoundModel::UploadAllModelTextureData(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& copycmdlist)
{
	if (!NeedToUploadTextureInfos())
	{
		return;
	}

	for (int i = 0; i < m_textureuploadInfos.size(); i++)
	{
		UploadModelTextureData(m_textureuploadInfos[i],creationdevice,copycmdlist);
		/*if (!m_textureuploadInfos[i].dataloaded)
		{
			m_textureuploadInfos[i].ModelMaterialholdingTexture->LoadTextureData(m_textureuploadInfos[i].texture);
			m_textureuploadInfos[i].dataloaded = true;
		}
		if (m_textureuploadInfos[i].needTextureInit)
		{
			m_textureuploadInfos[i].texture->Init(creationdevice);
			//srv entries need to be updated which are dependent on loaded texture data(make sure these properties are not set beforehard as thry will be invalid before loading)
			m_textureuploadInfos[i].srvdesc.Texture2D.MipLevels = m_textureuploadInfos[i].texture->GetTotalMipCount();
			m_textureuploadInfos[i].srvdesc.Format = m_textureuploadInfos[i].texture->GetDXImageData().m_imagemetadata.format;
			m_textureuploadInfos[i].needTextureInit = false;
			if (m_textureuploadInfos[i].createSRVfrominfo)
			{
				m_textureuploadInfos[i].texture->CreateSRV(creationdevice, m_textureuploadInfos[i].srvdesc, m_textureuploadInfos[i].srvCreationDescHandle);
				m_textureuploadInfos[i].createSRVfrominfo = false;
			}
		}
		m_textureuploadInfos[i].texture->UploadTexture(copycmdlist);*/

	}
	//update currenttexidxtoupload variable such that internal function can indicate no textures need upload. 
	m_currenttexidxtoupload = m_textureuploadInfos.size();
}
void CompoundModel::UploadModelTextureData(ModelMaterial::TextureUploadInfo& texUploadinfo, ComPtr< ID3D12Device> creationdevice, DX12Commandlist& copycmdlist)
{
	if (!texUploadinfo.dataloaded)
	{
		texUploadinfo.ModelMaterialholdingTexture->LoadTextureData(texUploadinfo.texture);
		texUploadinfo.dataloaded = true;
	}
	if (texUploadinfo.needTextureInit)
	{
		texUploadinfo.texture->Init(creationdevice);
		//srv entries need to be updated which are dependent on loaded texture data(make sure these properties are not set beforehard as thry will be invalid before loading)
		texUploadinfo.srvdesc.Texture2D.MipLevels = texUploadinfo.texture->GetTotalMipCount();
		texUploadinfo.srvdesc.Format = texUploadinfo.texture->GetDXImageData().m_imagemetadata.format;
		texUploadinfo.needTextureInit = false;
		if (texUploadinfo.createSRVfrominfo)
		{
			texUploadinfo.texture->CreateSRV(creationdevice, texUploadinfo.srvdesc, texUploadinfo.srvCreationDescHandle);
			texUploadinfo.createSRVfrominfo = false;
		}
	}
	texUploadinfo.texture->UploadTexture(copycmdlist);
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
			amodel->GetMaterialTexturesUploadInfo(m_textureuploadInfos);
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
			if (amodel->GetLoadedMaterial().GetRoughnessTexture())
			{
				numsrvrequired += 1;
			}
			//roughness/metalness(note we  can have roughness/metalness to be packed in single texture.
			if (amodel->GetLoadedMaterial().GetMetalnessTexture())
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
					//deferring loading texture data at creation time hence full srv creation data cannot be determined(mip level format not set)
					srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					//we update srv creation for this texture to upload info as we are differing the loading and actual resource creation of texture. 
					UpdateTexUploadInfoSRVcreationData(m_textureuploadInfos, diffusetex, srvdesc, m_resourceviewheap.GetCPUHandleOffseted(indexinsrvheaptouse));

					m_models[i]->SetDiffusetextureIdx(indexinsrvheaptouse);
					indexinsrvheaptouse++;

				}
				//handle normal texture
				DXTexture* normaltex = m_models[i]->GetLoadedMaterial().GetNormalTexture();
				if (normaltex)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
					//deferring loading texture data at creation time hence full srv creation data cannot be determined(mip level format not set)
					srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					//we update srv creation for this texture to upload info as we are differing the loading and actual resource creation of texture. 
					UpdateTexUploadInfoSRVcreationData(m_textureuploadInfos, normaltex, srvdesc, m_resourceviewheap.GetCPUHandleOffseted(indexinsrvheaptouse));
					m_models[i]->SetNormaltextureIdx(indexinsrvheaptouse);
					indexinsrvheaptouse++;

				}
				//handle roughness texture
				DXTexture* roughnesstex = m_models[i]->GetLoadedMaterial().GetRoughnessTexture();
				if (roughnesstex)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
					//deferring loading texture data at creation time hence full srv creation data cannot be determined(mip level format not set)
					srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					srvdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					//we update srv creation for this texture to upload info as we are differing the loading and actual resource creation of texture. 
					UpdateTexUploadInfoSRVcreationData(m_textureuploadInfos, roughnesstex, srvdesc, m_resourceviewheap.GetCPUHandleOffseted(indexinsrvheaptouse));
					m_models[i]->SetRoughnesstextureIdx(indexinsrvheaptouse);
					indexinsrvheaptouse++;

				}
				//handle metalness texture
				DXTexture* metalnesstex = m_models[i]->GetLoadedMaterial().GetMetalnessTexture();
				if (metalnesstex)
				{
					//not implemented yet currently we exprect metalnesstexture to be combined with roughness texture.
					assert(false);

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
void CompoundModel::UpdateTexUploadInfoSRVcreationData(vector< ModelMaterial::TextureUploadInfo>& texUploadInfos, DXTexture* targetTexture, D3D12_SHADER_RESOURCE_VIEW_DESC& srvdesc, D3D12_CPU_DESCRIPTOR_HANDLE srvDescHandle)
{
	for (ModelMaterial::TextureUploadInfo& texUploadInfo : texUploadInfos)
	{
		if (texUploadInfo.texture == targetTexture)
		{
			texUploadInfo.createSRVfrominfo = true;
			texUploadInfo.srvdesc = srvdesc;
			texUploadInfo.srvCreationDescHandle = srvDescHandle;
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
void BasicModelManager::InitCubeModel(ComPtr< ID3D12Device> creationdevice, Model& cubemodel, VertexVersion vertexVersion)
{
	vector<VertexBase*> verticies;
	if (vertexVersion == VertexVersion::VERTEXVERSION0)
	{
		GetCubeVerticiesV0(verticies);
		cubemodel.SetVertexVersionUsed(VERTEXVERSION0);
		cubemodel.InitVertexBuffer(creationdevice, verticies);
		cubemodel.InitIndexBuffer(creationdevice, cubeindicies);
	}
	else if (vertexVersion == VertexVersion::VERTEXVERSION3)
	{
		GetCubeVerticiesV3(verticies);
		cubemodel.SetVertexVersionUsed(VertexVersion::VERTEXVERSION3);
		cubemodel.InitVertexBuffer(creationdevice, verticies);
	}
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
	AssimpManager assimpmodel(modelfilepath, texfilepath);
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
void BasicModelManager::GetCubeVerticiesV3(vector<VertexBase*>& outverticies)
{
	int numvalues = sizeof(cubeverticicesV3) / sizeof(float);
	const int numValuesperVertex = 6;
	int numverticies = numvalues/ numValuesperVertex;
	for (int i = 0; i < numverticies; i++)
	{
		float* ptrToVertexValues = cubeverticicesV3 + (i * numValuesperVertex);
		VertexV3* vert = new VertexV3();
		vert->m_position = { ptrToVertexValues[0],ptrToVertexValues[1],ptrToVertexValues[2] };
		vert->m_uv = { 0.0f,0.0f };
		vert->m_normal = { ptrToVertexValues[3],ptrToVertexValues[4],ptrToVertexValues[5] };
		outverticies.push_back(vert);
	}
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
	m_metalnesstexture(nullptr),
	m_diffusetextureDataLoaded(false),
	m_normaltextureDataLoaded(false),
	m_roughnesstextureDataLoaded(false),
	m_metalnesstextureDataLoaded(false)
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
	
	/*m_diffusetexture = new DXTexture(texname);
	wstring texpath=GetTextureFilePath(texname);
	DXTexManager::LoadTexture(texpath.c_str(), m_diffusetexture->GetDXImageData());*/
	CreateTextureFileInternal(m_diffusetexture, texname);
}
void ModelMaterial::LoadNormalTexture(std::wstring texname)
{
	
	/*m_normaltexture = new DXTexture(texname);
	wstring texpath = GetTextureFilePath(texname);
	DXTexManager::LoadTexture(texpath.c_str(), m_normaltexture->GetDXImageData());*/
	CreateTextureFileInternal(m_normaltexture, texname);
}
void ModelMaterial::LoadRoughnessTexture(std::wstring texname)
{

	/*m_roughnesstexture = new DXTexture(texname);
	wstring texpath = GetTextureFilePath(texname);
	DXTexManager::LoadTexture(texpath.c_str(), m_roughnesstexture->GetDXImageData());*/
	CreateTextureFileInternal(m_roughnesstexture, texname);
}
void ModelMaterial::LoadMetalnessTexture(std::wstring texname)
{

	/*m_metalnesstexture = new DXTexture(texname);
	wstring texpath = GetTextureFilePath(texname);
	DXTexManager::LoadTexture(texpath.c_str(), m_metalnesstexture->GetDXImageData());*/
	CreateTextureFileInternal(m_metalnesstexture, texname);
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
void ModelMaterial::Init(ComPtr< ID3D12Device> creationdevice, bool allowTextureDataloading)
{
	if (m_diffusetexture)
	{
		if (allowTextureDataloading && !m_diffusetextureDataLoaded)
		{
			LoadTextureDataInternal(m_diffusetexture);
			m_diffusetextureDataLoaded = true;
		}
		//m_diffusetexture->Init(creationdevice);
		
		//m_diffusetexture->SetName(L"diffusetexture_modelmaterial");
	}
	if (m_normaltexture)
	{
		if (allowTextureDataloading && !m_normaltextureDataLoaded)
		{
			LoadTextureDataInternal(m_normaltexture);
			m_normaltextureDataLoaded = true;
		}
		//m_normaltexture->Init(creationdevice);
		//m_diffusetexture->SetName(L"normaltexture_modelmaterial");
	}
	if (m_roughnesstexture)
	{
		if (allowTextureDataloading && !m_roughnesstextureDataLoaded)
		{
			LoadTextureDataInternal(m_roughnesstexture);
			m_roughnesstextureDataLoaded = true;
		}
		//m_roughnesstexture->Init(creationdevice);
		//m_roughnesstexture->SetName(L"roughnesstexture_modelmaterial");
	}
	if (m_metalnesstexture)
	{
		if (!m_metalnesstextureDataLoaded)
		{
			LoadTextureDataInternal(m_metalnesstexture);
			m_metalnesstextureDataLoaded = true;
		}
		//m_metalnesstexture->Init(creationdevice);
		//m_metalnesstexture->SetName(L"metalnesstexture_modelmaterial");
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
void ModelMaterial::GetMaterialTexturesUploadInfo(vector< TextureUploadInfo>& textureUploadInfos)
{
	if (m_diffusetexture)
	{
		TextureUploadInfo diffusetextureUploadInfo;
		diffusetextureUploadInfo.texture = m_diffusetexture;
		diffusetextureUploadInfo.dataloaded = m_diffusetextureDataLoaded;
		diffusetextureUploadInfo.ModelMaterialholdingTexture = this;
		diffusetextureUploadInfo.needTextureInit = true;
		textureUploadInfos.push_back(diffusetextureUploadInfo);
	}
	if (m_normaltexture)
	{
		TextureUploadInfo normaltextureUploadInfo;
		normaltextureUploadInfo.texture = m_normaltexture;
		normaltextureUploadInfo.dataloaded = m_normaltextureDataLoaded;
		normaltextureUploadInfo.ModelMaterialholdingTexture = this;
		normaltextureUploadInfo.needTextureInit = true;
		textureUploadInfos.push_back(normaltextureUploadInfo);
	}
	if (m_roughnesstexture)
	{
		TextureUploadInfo roughnesstextureUploadInfo;
		roughnesstextureUploadInfo.texture = m_roughnesstexture;
		roughnesstextureUploadInfo.dataloaded = m_roughnesstextureDataLoaded;
		roughnesstextureUploadInfo.ModelMaterialholdingTexture = this;
		roughnesstextureUploadInfo.needTextureInit = true;

		textureUploadInfos.push_back(roughnesstextureUploadInfo);
	}
	if (m_metalnesstexture)
	{
		TextureUploadInfo metalnesstextureUploadInfo;
		metalnesstextureUploadInfo.texture = m_metalnesstexture;
		metalnesstextureUploadInfo.dataloaded = m_metalnesstextureDataLoaded;
		metalnesstextureUploadInfo.ModelMaterialholdingTexture = this;
		metalnesstextureUploadInfo.needTextureInit = true;
		textureUploadInfos.push_back(metalnesstextureUploadInfo);
	}
}
void ModelMaterial::CreateTextureFileInternal(DXTexture*& desttexptr, std::wstring texname)
{
	assert(desttexptr == nullptr);
	desttexptr = new DXTexture(texname);
	wstring texpath = GetTextureFilePath(texname);
	//DXTexManager::LoadTexture(texpath.c_str(), desttexptr->GetDXImageData());
}
void ModelMaterial::LoadTextureDataInternal(DXTexture*& texptr)
{
	assert(texptr != nullptr);
	wstring texpath = GetTextureFilePath(texptr->GetExternalTextureFileName());
	DXTexManager::LoadTexture(texpath.c_str(), texptr->GetDXImageData());
}
void ModelMaterial::LoadTextureData(DXTexture* texptr)
{
	LoadTextureDataInternal(texptr);
	if (texptr == m_diffusetexture)
	{
		m_diffusetextureDataLoaded = true;
	}
	else if (texptr == m_normaltexture)
	{
		m_normaltextureDataLoaded = true;
	}
	else if (texptr == m_roughnesstexture)
	{
		m_roughnesstextureDataLoaded = true;
	}
	else if (texptr == m_metalnesstexture)
	{
		m_metalnesstextureDataLoaded = true;
	}
}