#include"RaytracingUtils.h"

void ModelAccelerationStructure::Init(Model& modeltoprocess)
{
	//we need ib &vb in gpu memory.
	assert(modeltoprocess.GetUploadMode() == ModelDataUploadMode::COPY);
	m_rtgeometrydesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE::D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	//fixed in the current frame work(index is unsigned int)
	m_rtgeometrydesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
	m_rtgeometrydesc.Triangles.IndexBuffer = modeltoprocess.GetIertexBufferGPUVirtualAddress();
	m_rtgeometrydesc.Triangles.IndexCount = (UINT)modeltoprocess.GetIndiciesCount();

	m_rtgeometrydesc.Triangles.VertexBuffer.StartAddress = modeltoprocess.GetVertexBufferGPUVirtualAddress();
	
	//stride is the same as vertex size used in model
	m_rtgeometrydesc.Triangles.VertexBuffer.StrideInBytes = DXVertexManager::GetVertexSize(modeltoprocess.GetVertexVersionUsed());
	//a vertex position always has same format no matter the vertex version used
	m_rtgeometrydesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	m_rtgeometrydesc.Triangles.VertexCount = modeltoprocess.GetVerticiesCount();
	m_rtgeometrydesc.Triangles.Transform3x4 = NULL;
	
	
}

void ModelAccelerationStructure::Build(ComPtr< ID3D12Device5> device)
{
	 
	{
		m_asinputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE::D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		m_asinputs.DescsLayout = D3D12_ELEMENTS_LAYOUT::D3D12_ELEMENTS_LAYOUT_ARRAY;
		
		m_asinputs.pGeometryDescs = &m_rtgeometrydesc;
		m_asinputs.NumDescs = 1;
		m_asinputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS::D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	}
	
	
	device->GetRaytracingAccelerationStructurePrebuildInfo(&m_asinputs, &m_prebuildinfo);
	m_accelerationstucturescratch.Init(device, m_prebuildinfo.ScratchDataSizeInBytes, true);
	m_accelerationstructure.Init(device, m_prebuildinfo.ResultDataMaxSizeInBytes);
	

	
}
void ModelAccelerationStructure::IssueBuild(ComPtr<ID3D12GraphicsCommandList4>buildcmdlist)
{
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC builddesc = {};
	builddesc.DestAccelerationStructureData = m_accelerationstructure.GetResource()->GetGPUVirtualAddress();
	builddesc.ScratchAccelerationStructureData = m_accelerationstucturescratch.GetResource()->GetGPUVirtualAddress();
	builddesc.Inputs = m_asinputs;
	buildcmdlist->BuildRaytracingAccelerationStructure(&builddesc, 0, nullptr);
}

void AccelerationStructureResource::Init(ComPtr< ID3D12Device5> device, UINT64 size,bool isscratch)
{
	m_resdesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	m_resdesc.Alignment = 0;
	m_resdesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	m_resdesc.Height = 1;
	m_resdesc.DepthOrArraySize = 1;
	m_resdesc.MipLevels = 1;
	m_resdesc.Width = size;
	m_resdesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	m_resdesc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	
	D3D12_HEAP_PROPERTIES defaultheapprops = {};
	defaultheapprops.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	defaultheapprops.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	defaultheapprops.VisibleNodeMask = 0;
	defaultheapprops.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	if (isscratch)
	{
		m_currentresstate = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
	}
	else
	{
		m_currentresstate = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	}
	DXASSERT(device->CreateCommittedResource(&defaultheapprops,D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,&m_resdesc,m_currentresstate,nullptr,IID_PPV_ARGS(m_resource.GetAddressOf())))
}