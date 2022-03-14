#include"DX12Resource.h"


D3D12_RESOURCE_BARRIER DX12ResourceBase::TransitionResState(D3D12_RESOURCE_STATES targetstate)
{
	assert(targetstate != m_currentresstate);
	D3D12_RESOURCE_BARRIER transitionbarrier = {};
	//return a default barrier struct if target state is same as current state
	if (targetstate != m_currentresstate)
	{
		transitionbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		transitionbarrier.Transition.StateBefore = m_currentresstate;
		transitionbarrier.Transition.StateAfter = targetstate;
		transitionbarrier.Transition.pResource = m_resource.Get();
		m_currentresstate = targetstate;
	}
	return transitionbarrier;
}

void DX12Resource::CreateUAV(ComPtr< ID3D12Device> creationdevice, D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle)
{
	creationdevice->CreateUnorderedAccessView(m_resource.Get(), nullptr, &uavdesc, uavhandle);
}

void DX12Resource::CreateSRV(ComPtr< ID3D12Device> creationdevice,D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc,D3D12_CPU_DESCRIPTOR_HANDLE srvhandle)
{
	creationdevice->CreateShaderResourceView(m_resource.Get(), &srvdesc, srvhandle);
	
}

void DX12Resource::Init(ComPtr< ID3D12Device> creationdevice, DX12ResourceCreationProperties resprops, ResourceCreationMode creationmode)
{
	m_creationproperties = resprops;
	m_rescreationmode = creationmode;
	SetResState(m_creationproperties.resinitialstate);
	D3D12_CLEAR_VALUE* optimizedclearvalue = nullptr; &m_creationproperties.optimizedclearvalue;
	if (m_creationproperties.useclearvalue)
		optimizedclearvalue = &m_creationproperties.optimizedclearvalue;

	switch (m_rescreationmode)
	{
	case ResourceCreationMode::COMMITED:
		DXASSERT(creationdevice->CreateCommittedResource(&m_creationproperties.resheapprop,m_creationproperties.resheapflags,&m_creationproperties.resdesc,m_creationproperties.resinitialstate,optimizedclearvalue,IID_PPV_ARGS(m_resource.GetAddressOf())))
		break;

	default:
		break;
	}
}

void DX12Resource::InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops)
{
	rescreationprops.useclearvalue = false;
	rescreationprops.resinitialstate = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	rescreationprops.resheapprop.Type = D3D12_HEAP_TYPE_DEFAULT;
	rescreationprops.resheapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	rescreationprops.resheapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	rescreationprops.resheapflags = D3D12_HEAP_FLAG_NONE;
	rescreationprops.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	rescreationprops.resdesc.Alignment = 0;
	rescreationprops.resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	rescreationprops.resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	rescreationprops.resdesc.DepthOrArraySize = 1;
	rescreationprops.resdesc.MipLevels = 1;
	rescreationprops.resdesc.SampleDesc.Count = 1;
	rescreationprops.resdesc.SampleDesc.Quality = 0;
	rescreationprops.resdesc.Height = 1;
	rescreationprops.resdesc.Width = 1;
	rescreationprops.resdesc.Format = DXGI_FORMAT_UNKNOWN;
	rescreationprops.resheapprop.CreationNodeMask = 0;
	rescreationprops.resheapprop.VisibleNodeMask = 0;
}

void DX12ReservedResource::UploadTextureData(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& copycmdlist)
{

}

void DX12ReservedResource::Init(ComPtr< ID3D12Device> creationdevice, DX12ResourceCreationProperties resprops)
{
	m_creationproperties = resprops;
	m_rescreationmode = ResourceCreationMode::RESERVED;
	SetResState(m_creationproperties.resinitialstate);

	D3D12_CLEAR_VALUE* clearvalue = nullptr;
	if (m_creationproperties.useclearvalue)
	{
		clearvalue = &m_creationproperties.optimizedclearvalue;
	}

	UINT numtilesforfullresource;
	//NOTE:using mipcounts as total subresources in the reserved res(might need to change if needed)
	UINT numsubresourcetiling = m_creationproperties.resdesc.MipLevels;
	m_subresourcetiling.resize(numsubresourcetiling);
	DXASSERT(creationdevice->CreateReservedResource(&m_creationproperties.resdesc, m_creationproperties.resinitialstate, clearvalue, IID_PPV_ARGS(m_resource.GetAddressOf())))
		//retrive resource tiling info as well
		creationdevice->GetResourceTiling(m_resource.Get(), &numtilesforfullresource, &m_packedmipinfo, nullptr, &numsubresourcetiling,0,m_subresourcetiling.data());

}
void DX12ReservedResource::CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle)
{
	creationdevice->CreateShaderResourceView(m_resource.Get(), &srvdesc, srvhandle);
}
void DX12ReservedResource::CreateUAV(ComPtr< ID3D12Device> creationdevice, D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle)
{
	creationdevice->CreateUnorderedAccessView(m_resource.Get(), nullptr, &uavdesc, uavhandle);
}
DX12ReservedResource::DX12ReservedResource()
{}
DX12ReservedResource::~DX12ReservedResource()
{}

void DX12ReservedResource::InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops)
{
	//same as any generic resource
	DX12Resource::InitResourceCreationProperties(rescreationprops);
	//some special properties for reserved resource
	rescreationprops.resdesc.Layout = D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
}



DX12ReservedresourcePhysicalMemoryManager::DX12ReservedresourcePhysicalMemoryManager()
	:m_reservedres(nullptr)
{}

DX12ReservedresourcePhysicalMemoryManager::~DX12ReservedresourcePhysicalMemoryManager()
{}

void DX12ReservedresourcePhysicalMemoryManager::Init(ComPtr< ID3D12Device> creationdevice, DX12ReservedResource* resourcetomanage)
{
	m_reservedres = resourcetomanage;

	const D3D12_PACKED_MIP_INFO& resmipinfo=m_reservedres->GetPackedMipInfo();
	UINT heapsrequiredforres = resmipinfo.NumStandardMips;

	if (resmipinfo.NumPackedMips > 0)
	{
		//1 heap for all packedmips(technique taken from microsoft example)
		heapsrequiredforres++;
	}

		//prepare subres info needed for upload
		const vector<D3D12_SUBRESOURCE_TILING> subrestilinginfo=m_reservedres->GetSubResourceTilingInfo();
		m_subresourceinfo.resize(subrestilinginfo.size());
		m_heaps.resize(heapsrequiredforres);

		m_subresourceinfo.resize(subrestilinginfo.size());
		for (size_t i = 0; i < subrestilinginfo.size(); i++)
			
		{
			const D3D12_SUBRESOURCE_TILING& subrestiling = subrestilinginfo[i];
			if (i < resmipinfo.NumStandardMips)
			{
				
				//init as unpacked mip
				m_subresourceinfo[i].heapindex = i;
				m_subresourceinfo[i].coordinates.Subresource = i;
				m_subresourceinfo[i].coordinates.X = 0;
				m_subresourceinfo[i].coordinates.Y = 0;
				m_subresourceinfo[i].coordinates.Z = 0;
				m_subresourceinfo[i].tileregionsize.Width = subrestiling.WidthInTiles;
				m_subresourceinfo[i].tileregionsize.Height = subrestiling.HeightInTiles;
				m_subresourceinfo[i].tileregionsize.Depth = subrestiling.DepthInTiles;
				m_subresourceinfo[i].tileregionsize.NumTiles = subrestiling.HeightInTiles* subrestiling.WidthInTiles* subrestiling.DepthInTiles;
				m_subresourceinfo[i].tileregionsize.UseBox = TRUE;
				//init respectiveheap
				InitHeap(i, creationdevice, m_subresourceinfo[i].tileregionsize.NumTiles * D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);


				
			}
			else
			{
				//init as packed mip
				m_subresourceinfo[i].ispacked = true;
				m_subresourceinfo[i].heapindex = heapsrequiredforres-1;//last heap for all packed mips
				m_subresourceinfo[i].coordinates.Subresource = heapsrequiredforres - 1;//all packed regions must specify the last packed mip subresindex
				m_subresourceinfo[i].coordinates.X = 0;
				m_subresourceinfo[i].coordinates.Y = 0;
				m_subresourceinfo[i].coordinates.Z = 0;
				
				m_subresourceinfo[i].tileregionsize.NumTiles = resmipinfo.NumTilesForPackedMips;
				m_subresourceinfo[i].tileregionsize.UseBox = FALSE;

				//init respectiveheap
				InitHeap(heapsrequiredforres - 1, creationdevice, m_subresourceinfo[i].tileregionsize.NumTiles * D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
			}
		
		}

		//init uav heaps
		D3D12_DESCRIPTOR_HEAP_DESC uavheapdesc = {};
		uavheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		uavheapdesc.NodeMask = 0;
		uavheapdesc.NumDescriptors = m_reservedres->GetTotalMipCount();
		uavheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		m_reservedresheap_upload.Init(uavheapdesc, creationdevice);
		uavheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_reservedresheap.Init(uavheapdesc, creationdevice);

		//1 uav for each mip
		for(size_t i=0;i<m_reservedres->GetTotalMipCount();i++)
		
		{
			D3D12_CPU_DESCRIPTOR_HANDLE uavhandle = m_reservedresheap_upload.GetCPUHandleOffseted(i);
			D3D12_UNORDERED_ACCESS_VIEW_DESC reservedresuavdesc = {};
			reservedresuavdesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			reservedresuavdesc.Texture2D.MipSlice = i;
			m_reservedres->CreateUAV(creationdevice, reservedresuavdesc, uavhandle);
		}
		creationdevice->CopyDescriptorsSimple(m_reservedres->GetTotalMipCount(), m_reservedresheap.GetCPUHandlefromstart(), m_reservedresheap_upload.GetCPUHandlefromstart(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		
		
		

		
}

void DX12ReservedresourcePhysicalMemoryManager::InitHeap(size_t indexinheapvector, ComPtr< ID3D12Device> creationdevice, UINT64 heapsize)
{
	if (m_heaps[indexinheapvector].Get())
	{
		//heap is already created do not recreate.
		return;
	}
	D3D12_HEAP_DESC heapdesc = {};
	heapdesc.Alignment = 0;
	heapdesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
	heapdesc.SizeInBytes = heapsize;
	heapdesc.Properties.VisibleNodeMask = 0;
	heapdesc.Properties.CreationNodeMask = 0;
	heapdesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapdesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

	DXASSERT(creationdevice->CreateHeap(&heapdesc, IID_PPV_ARGS(m_heaps[indexinheapvector].GetAddressOf())))
		DXASSERT(m_heaps[indexinheapvector]->SetName(L"ReservedResourceheap"))
}

void DX12ReservedresourcePhysicalMemoryManager::ClearReservedResource(DX12Commandlist& cmdlist)
{
	D3D12_GPU_DESCRIPTOR_HANDLE uavhandle_gpu[9];
	D3D12_CPU_DESCRIPTOR_HANDLE uavhandle_cpu[9];
	FLOAT clearvalue_white[4] = { 1.0f,1.0f,1.0f,1.0f };
	FLOAT clearvalue_green[4] = { 0.0f,1.0f,0.0f,1.0f };
	FLOAT clearvalue_red[4] = { 1.0f,0.0f,0.0f,1.0f };
	FLOAT clearvalue_blue[4] = { 0.0f,0.0f,1.0f,1.0f };
	for (size_t i = 0; i < 9; i++)
	{
		uavhandle_gpu[i] = m_reservedresheap.GetGPUHandleOffseted(i);
		uavhandle_cpu[i] = m_reservedresheap_upload.GetCPUHandleOffseted(i);
	}
	 
	ID3D12DescriptorHeap* heapstoset[1] =
	{
		m_reservedresheap.GetDescHeap()
	};
	cmdlist->SetDescriptorHeaps(1, heapstoset);
		cmdlist->ClearUnorderedAccessViewFloat(uavhandle_gpu[0],uavhandle_cpu[0],m_reservedres->GetResource().Get(),clearvalue_white,0,nullptr);
		cmdlist->ClearUnorderedAccessViewFloat(uavhandle_gpu[1], uavhandle_cpu[1], m_reservedres->GetResource().Get(), clearvalue_green, 0, nullptr);
		cmdlist->ClearUnorderedAccessViewFloat(uavhandle_gpu[2], uavhandle_cpu[2], m_reservedres->GetResource().Get(), clearvalue_red, 0, nullptr);
		cmdlist->ClearUnorderedAccessViewFloat(uavhandle_gpu[8], uavhandle_cpu[8], m_reservedres->GetResource().Get(), clearvalue_blue, 0, nullptr);
}

void DX12ReservedresourcePhysicalMemoryManager::UpdateReservedresourcePhysicalMemoryMappings(ComPtr<ID3D12CommandQueue> commandqueue)
{
	D3D12_TILE_RANGE_FLAGS rangemappingflags;
	D3D12_TILED_RESOURCE_COORDINATE regioncoordinatestomap;
	D3D12_TILE_REGION_SIZE regionsizetomap;
	UINT rangestartoffset,rangetilecount;
	ID3D12Heap* heapstomap;
	UINT resourceregionstomap = 1;
	for (size_t i = 0; i < m_subresourceinfo.size(); i++)
	{
		
		rangestartoffset = 0;
		rangemappingflags=D3D12_TILE_RANGE_FLAGS::D3D12_TILE_RANGE_FLAG_NONE;
		const SubResouceInfo& subresinfo = m_subresourceinfo[i];
		//if (subresinfo.ispacked) { continue; }
		ComPtr<ID3D12Heap> heap = m_heaps[subresinfo.heapindex];
		heapstomap=heap.Get();
		rangetilecount=subresinfo.tileregionsize.NumTiles;
		regionsizetomap=(subresinfo.tileregionsize);
		regioncoordinatestomap=subresinfo.coordinates;
		commandqueue->UpdateTileMappings(m_reservedres->GetResource().Get(), resourceregionstomap, &regioncoordinatestomap, &regionsizetomap, heapstomap, resourceregionstomap, &rangemappingflags, &rangestartoffset, &rangetilecount, D3D12_TILE_MAPPING_FLAG_NONE);
	}
	{
		
		
	}
}