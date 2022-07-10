#include"RaytracingUtils.h"

ModelAccelerationStructureBLAS::ModelAccelerationStructureBLAS()
	:m_buildcmdissued(false)
{

}
void ModelAccelerationStructureBLAS::Init(Model& modeltoprocess)
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

void ModelAccelerationStructureBLAS::Build(ComPtr< ID3D12Device5> device)
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
	m_accelerationstucturescratch.SetName(L"AccelerationBLASScratch");
	m_accelerationstructure.Init(device, m_prebuildinfo.ResultDataMaxSizeInBytes);
	m_accelerationstructure.SetName(L"AccelerationStructureBLAS");
	

	
}
void ModelAccelerationStructureBLAS::IssueBuild(ComPtr<ID3D12GraphicsCommandList4>buildcmdlist)
{
	//build cmd should be issued once
	if (m_buildcmdissued)
	{
		return;
	}
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC builddesc = {};
	builddesc.DestAccelerationStructureData = m_accelerationstructure.GetResource()->GetGPUVirtualAddress();
	builddesc.ScratchAccelerationStructureData = m_accelerationstucturescratch.GetResource()->GetGPUVirtualAddress();
	builddesc.Inputs = m_asinputs;
	buildcmdlist->BuildRaytracingAccelerationStructure(&builddesc, 0, nullptr);
	//issue a uav barrier for it as well.
	D3D12_RESOURCE_BARRIER uavbarrier = {};
	uavbarrier.Type =D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavbarrier.UAV.pResource = m_accelerationstructure.GetResource().Get();
	buildcmdlist->ResourceBarrier(1, &uavbarrier);
	m_buildcmdissued = true;
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
	m_resdesc.SampleDesc.Count = 1;
	m_resdesc.SampleDesc.Quality = 0;
	
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

ModelAccelerationStructureTLAS::ModelAccelerationStructureTLAS()
	:m_buildcmdissued(false)
{

}
void ModelAccelerationStructureTLAS::Init(ComPtr< ID3D12Device> creationdevice,vector<D3D12_RAYTRACING_INSTANCE_DESC> instancedescs)
{
	m_instancedescs = instancedescs;
	//prepare instancedesc buffer
	{
		DX12ResourceCreationProperties bufferprops;
		DX12Buffer::InitResourceCreationProperties(bufferprops);
		bufferprops.resdesc.Width = sizeof(D3D12_RAYTRACING_INSTANCE_DESC);
		bufferprops.resdesc.Width *= m_instancedescs.size();
		bufferprops.resheapprop.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		

		m_instancedescbuffer.Init(creationdevice, bufferprops, ResourceCreationMode::COMMITED);
	}
	m_asinputs.InstanceDescs = m_instancedescbuffer.GetResource()->GetGPUVirtualAddress();
	m_asinputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS::D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	m_asinputs.NumDescs = instancedescs.size();
	//copyover instance desc data to buffer
	{
		BufferMapParams mapparams = {};
		mapparams.range.Begin = 0;
		mapparams.range.End = m_instancedescbuffer.GetSize();
		void* bufferptr=m_instancedescbuffer.Map(mapparams);
		void* srcptr = reinterpret_cast<void*>(m_instancedescs.data());
		size_t copysize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * m_instancedescs.size();
		memcpy(bufferptr, srcptr,copysize );
		mapparams.range.End = m_instancedescbuffer.GetSize();
		m_instancedescbuffer.UnMap(mapparams);
	}

}
void ModelAccelerationStructureTLAS::Build(ComPtr< ID3D12Device5> device)
{
	m_asinputs.Type= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE::D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	device->GetRaytracingAccelerationStructurePrebuildInfo(&m_asinputs, &m_prebuildinfo);
	m_accelerationstucturescratch.Init(device, m_prebuildinfo.ScratchDataSizeInBytes, true);
	m_accelerationstucturescratch.SetName(L"AccelerationTLASScratch");
	m_accelerationstructure.Init(device, m_prebuildinfo.ResultDataMaxSizeInBytes);
	m_accelerationstructure.SetName(L"AccelerationStructureTLAS");
}
void ModelAccelerationStructureTLAS::IssueBuild(ComPtr<ID3D12GraphicsCommandList4>buildcmdlist)
{
	//build cmd should be issued once
	if (m_buildcmdissued)
	{
		return;
	}
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC builddesc = {};
	builddesc.DestAccelerationStructureData = m_accelerationstructure.GetResource()->GetGPUVirtualAddress();
	builddesc.ScratchAccelerationStructureData = m_accelerationstucturescratch.GetResource()->GetGPUVirtualAddress();
	builddesc.Inputs = m_asinputs;
	buildcmdlist->BuildRaytracingAccelerationStructure(&builddesc, 0, nullptr);
	m_buildcmdissued = true;
	D3D12_RESOURCE_BARRIER uavbarrier = {};
	uavbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavbarrier.UAV.pResource = m_accelerationstructure.GetResource().Get();
	buildcmdlist->ResourceBarrier(1, &uavbarrier);
}

void RaytracingCommon::InitAsIdentityMatrix(FLOAT arr[3][4])
{
	for (unsigned r = 0; r < 3; r++)
	{
		for (unsigned c = 0; c < 4; c++)
		{
			arr[r][c] = 0.0f;
		}
	}
	arr[0][0] = arr[1][1]= arr[2][2]= 1.0f;
}

void ModelAccelerationStructureTLAS::CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvdesc.RaytracingAccelerationStructure.Location = m_accelerationstructure.GetResource()->GetGPUVirtualAddress();
	srvdesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	creationdevice->CreateShaderResourceView(nullptr, &srvdesc, srvhandle);
}