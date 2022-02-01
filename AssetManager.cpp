#include"AssetManager.h"

std::vector<Vertex> planeverticies =
{
	{-1.0f,1.0f,0.0f,0.0,0.0f},	//0(left-top)
	{-1.0f,-1.0f,0.0f,0.0f,1.0f},	//1(left-bottom)
	{1.0f,-1.0f,0.0f,1.0f,1.0f},	//2(right-bottom)
	{1.0f,1.0f,0.0f,1.0f,0.0f}	//3(right-top)

};

std::vector<unsigned> planeindicies =
{
	0,1,2,
	0,2,3
};

//repesentation of a single mesh vertex


Model::Model()
{
}

Model::~Model()
{
}

void Model::InitVertexBuffer(ComPtr< ID3D12Device> creationdevice,vector<Vertex>& verticies)
{

	m_verticies = verticies;
	BufferCreationProperties vbproperties;
	vbproperties.resheapflags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE;
	vbproperties.resinitialstate = D3D12_RESOURCE_STATE_GENERIC_READ;
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
	vbproperties.resdesc.Width = sizeof(verticies[0])*(UINT64)verticies.size();
	vbproperties.resdesc.Format = DXGI_FORMAT_UNKNOWN;
	vbproperties.resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vbproperties.resdesc.SampleDesc.Count = 1;
	vbproperties.resdesc.SampleDesc.Quality = 0;
	vbproperties.resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	
	

	//prepare vbview as well
	m_vertexbuffer.Init(creationdevice, vbproperties, ResourceCreationMode::COMMITED);
	m_vertexbuffer.SetName(L"planeVB");
	m_vertexbufferview.BufferLocation = m_vertexbuffer.GetResource()->GetGPUVirtualAddress();
	m_vertexbufferview.SizeInBytes = m_vertexbuffer.GetSize();
	m_vertexbufferview.StrideInBytes=sizeof(Vertex);



}
void Model::InitIndexBuffer(ComPtr< ID3D12Device> creationdevice,vector<unsigned>& indicies)
{
	m_indicies = indicies;
	BufferCreationProperties ibproperties;
	ibproperties.resheapflags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE;
	ibproperties.resinitialstate = D3D12_RESOURCE_STATE_GENERIC_READ;
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
	m_indexbuffer.Init(creationdevice, ibproperties, ResourceCreationMode::COMMITED);
	m_indexbuffer.SetName(L"planeIB");
	
	//prepare index buffer view
	m_indexbufferview.BufferLocation = m_indexbuffer.GetResource()->GetGPUVirtualAddress();
	m_indexbufferview.SizeInBytes = m_indexbuffer.GetSize();
	m_indexbufferview.Format = DXGI_FORMAT_R32_UINT;




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

	void* vbmapped = m_vertexbuffer.Map(vbmapparams);
	memcpy(vbmapped, m_verticies.data(), m_vertexbuffer.GetSize());
	//written to full range
	vbmapparams.range.End = m_vertexbuffer.GetSize();
	m_vertexbuffer.UnMap(vbmapparams);

	}

	//upload indexdata
	{
		BufferMapParams ibmapparams = {};
		ibmapparams.subresource = 0;
		//not reading
		ibmapparams.range.Begin = 0;
		ibmapparams.range.End = 0;

		void* ibmapped = m_indexbuffer.Map(ibmapparams);
		memcpy(ibmapped, m_indicies.data(), m_indexbuffer.GetSize());
		//written to full range
		ibmapparams.range.End = m_indexbuffer.GetSize();
		m_indexbuffer.UnMap(ibmapparams);

	}
}




void BasicModelManager::InitPlaneModel(ComPtr< ID3D12Device> creationdevice, Model& planemodel)
{
	
	planemodel.InitVertexBuffer(creationdevice, planeverticies);
	planemodel.InitIndexBuffer(creationdevice, planeindicies);
	planemodel.UploadModelDatatoBuffers();
}