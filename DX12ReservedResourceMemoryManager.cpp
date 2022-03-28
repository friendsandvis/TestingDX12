#include"DX12ReservedResourceMemoryManager.h"


DX12ReservedResourceMemoryManager::DX12ReservedResourceMemoryManager()
	:m_restomanage(nullptr)
{
}

DX12ReservedResourceMemoryManager::~DX12ReservedResourceMemoryManager()
{}

void DX12ReservedResourceMemoryManager::Init(DX12ReservedResource* reservedresourcetomanage)
{
	m_restomanage = reservedresourcetomanage;

	const D3D12_PACKED_MIP_INFO& resmipinfo = m_restomanage->GetPackedMipInfo();
	UINT heapsrequiredforres = resmipinfo.NumStandardMips;

	if (resmipinfo.NumPackedMips > 0)
	{
		//1 heap for all packedmips(technique taken from microsoft example)
		heapsrequiredforres++;
	}
	m_heaps.resize(heapsrequiredforres);
	const vector<D3D12_SUBRESOURCE_TILING> subrestilinginfo = m_restomanage->GetSubResourceTilingInfo();
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
			m_subresourceinfo[i].tileregionsize.NumTiles = subrestiling.HeightInTiles * subrestiling.WidthInTiles * subrestiling.DepthInTiles;
			m_subresourceinfo[i].tileregionsize.UseBox = TRUE;



		}
		else
		{
			//init as packed mip
			m_subresourceinfo[i].ispacked = true;
			m_subresourceinfo[i].heapindex = heapsrequiredforres - 1;//last heap for all packed mips
			m_subresourceinfo[i].coordinates.Subresource = heapsrequiredforres - 1;//all packed regions must specify the last packed mip subresindex
			m_subresourceinfo[i].coordinates.X = 0;
			m_subresourceinfo[i].coordinates.Y = 0;
			m_subresourceinfo[i].coordinates.Z = 0;

			m_subresourceinfo[i].tileregionsize.NumTiles = resmipinfo.NumTilesForPackedMips;
			m_subresourceinfo[i].tileregionsize.UseBox = FALSE;

		}

	}
	//initiialize uav heaps for
	{

	}
}

void DX12ReservedResourceMemoryManager::Update(ComPtr<ID3D12CommandQueue>commandqueue, ComPtr< ID3D12Device> creationdevice)
{
	vector<UINT> subrestomap, subrestounmap;
	//map the unmapped region & vice versa(heap managment done as needed
	for (size_t i = 0; i < m_dirtysubres.size(); i++)
	{
		UINT targetsubresidx = m_dirtysubres[i];
		SubResouceInfo& subresinfo=m_subresourceinfo[targetsubresidx];
		if (subresinfo.isMapped && subresinfo.isunmapable)
		{
			//need to unmap as it's already mapped
			subrestounmap.push_back(targetsubresidx);
			subresinfo.isMapped = false;
		}
		else
		{
			//need to map as it's not mapped
			//create the corresponding heap as we will need it now
			InitHeap(subresinfo.heapindex, creationdevice, (UINT64)subresinfo.tileregionsize.NumTiles* D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
			subrestomap.push_back(targetsubresidx);
			subresinfo.isMapped = true;
		}
	}
	//empty dirtysubres vector now as we have processed all dirty onse.
	m_dirtysubres.clear();

	//unbind all heaps not needed
	for (size_t i = 0; i < subrestounmap.size(); i++)
	{
		UINT targetsubresidx = subrestounmap[i];
		SubResouceInfo& subresinfo = m_subresourceinfo[targetsubresidx];
		
		assert(subresinfo.isunmapable);
		bool heapdeletesuccess= DeleteHeap(subresinfo.heapindex);
		if (heapdeletesuccess)
		{
			D3D12_TILE_RANGE_FLAGS rangeflag = D3D12_TILE_RANGE_FLAGS::D3D12_TILE_RANGE_FLAG_NULL;
			commandqueue->UpdateTileMappings(m_restomanage->GetResource().Get(), 1, &subresinfo.coordinates, &subresinfo.tileregionsize, nullptr, 1, &rangeflag, nullptr, &subresinfo.tileregionsize.NumTiles, D3D12_TILE_MAPPING_FLAG_NONE);
		}

	}

	//bind heaps needed
	for (size_t i = 0; i < subrestomap.size(); i++)
	{
		UINT targetsubresidx = subrestomap[i];
		SubResouceInfo& subresinfo = m_subresourceinfo[targetsubresidx];
		D3D12_TILE_RANGE_FLAGS rangeflag = D3D12_TILE_RANGE_FLAGS::D3D12_TILE_RANGE_FLAG_NONE;
		UINT heapoffset = 0;
		//make sure heap exists
		assert(m_heaps[subresinfo.heapindex].heap.Get() != nullptr);
		commandqueue->UpdateTileMappings(m_restomanage->GetResource().Get(), 1, &subresinfo.coordinates, &subresinfo.tileregionsize, m_heaps[subresinfo.heapindex].heap.Get(), 1, &rangeflag, &heapoffset, &subresinfo.tileregionsize.NumTiles, D3D12_TILE_MAPPING_FLAG_NONE);
	}


}

void DX12ReservedResourceMemoryManager::InitHeap(size_t indexinheapvector, ComPtr< ID3D12Device> creationdevice, UINT64 heapsize)
{
	if (m_heaps[indexinheapvector].heap.Get())
	{
		//heap is already created do not recreate but increment heap usage count
		m_heaps[indexinheapvector].usagecount++;
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

	DXASSERT(creationdevice->CreateHeap(&heapdesc, IID_PPV_ARGS(m_heaps[indexinheapvector].heap.GetAddressOf())))
		DXASSERT(m_heaps[indexinheapvector].heap->SetName(L"ReservedResourceheap"))
		m_heaps[indexinheapvector].usagecount++;
}

bool DX12ReservedResourceMemoryManager::DeleteHeap(size_t heapindex)
{
	assert(heapindex < m_heaps.size());
	if (!m_heaps[heapindex].heap.Get())
	{
		//heap does not exists so nothing to do
		return false;
	}
	return m_heaps[heapindex].Delete();
}

void DX12ReservedResourceMemoryManager::BindMemory(UINT subresourceindex,bool makeunmapable)
{
	assert(subresourceindex < m_subresourceinfo.size());

	if (!m_subresourceinfo[subresourceindex].isMapped)
	{
		m_dirtysubres.push_back(subresourceindex);
	}
	if (makeunmapable)
	{
		m_subresourceinfo[subresourceindex].isunmapable = false;
	}

}
bool DX12ReservedResourceMemoryManager::UnbindMemory(UINT subresourceindex)
{
	assert(subresourceindex < m_subresourceinfo.size());
	if (m_subresourceinfo[subresourceindex].isMapped && m_subresourceinfo[subresourceindex].isunmapable)
	{
		m_dirtysubres.push_back(subresourceindex);
		return true;
	}
	return false;
}

bool DX12ReservedResourceMemoryManager::IsMemoryBound(UINT subresourceindex)
{
	assert(subresourceindex < m_subresourceinfo.size());
	return m_subresourceinfo[subresourceindex].isMapped;
}