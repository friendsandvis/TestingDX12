#include"DX12ReservedResourceMemoryManager.h"


DX12ReservedResourceMemoryManager::DX12ReservedResourceMemoryManager()
	:m_restomanage(nullptr),
	m_allowuploadonmapping(true)
{
}

DX12ReservedResourceMemoryManager::~DX12ReservedResourceMemoryManager()
{
	//delete upload buffers
	for (size_t i = 0; i < m_uploadbuffers.size(); i++)
	{
		delete m_uploadbuffers[i];
	}
	m_uploadbuffers.clear();
}

void DX12ReservedResourceMemoryManager::Init(ComPtr< ID3D12Device> creationdevice,DX12ReservedResource* reservedresourcetomanage,bool inituav)
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
	//initialize upload data upload utils
	DXImageData& resimgdata = m_restomanage->GetImageData();
	resimgdata.GetSubresData(creationdevice, m_reservedresourcesubresdata);
	UINT64 uploadbuffersize=GetRequiredIntermediateSize(m_restomanage->GetResource().Get(), 0, m_reservedresourcesubresdata.size());
	
	//UINT64 uploadbuffersize = GetRequiredIntermediateSize(m_restomanage->GetResource().Get(), 0, 1);

	DX12ResourceCreationProperties uploadbufferprops;
	DX12Buffer::InitResourceCreationProperties(uploadbufferprops);
	uploadbufferprops.resdesc.Width = uploadbuffersize;
	uploadbufferprops.resheapprop.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	uploadbufferprops.resinitialstate = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
	m_uploadbuffer.Init(creationdevice, uploadbufferprops, ResourceCreationMode::COMMITED);

	if (inituav)
	{
		D3D12_DESCRIPTOR_HEAP_DESC uavheapdesc = {};
		uavheapdesc.NumDescriptors = m_restomanage->GetTotalMipCount();
		uavheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		uavheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_uavheapupload.Init(uavheapdesc, creationdevice);
		uavheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_uavheap.Init(uavheapdesc, creationdevice);
		for (unsigned i = 0; i < m_restomanage->GetTotalMipCount(); i++)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc = {};
			uavdesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavdesc.Texture2D.MipSlice = i;
			uavdesc.Texture2D.PlaneSlice = 0;
			m_restomanage->CreateUAV(creationdevice, uavdesc, m_uavheapupload.GetCPUHandleOffseted(i));
		}
		creationdevice->CopyDescriptorsSimple(m_restomanage->GetTotalMipCount(), m_uavheap.GetCPUHandlefromstart(), m_uavheapupload.GetCPUHandlefromstart(), D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}

void DX12ReservedResourceMemoryManager::RequestUploadData(UINT subresindextoupload)
{
	m_subrestoupload.push_back(subresindextoupload);
}

void DX12ReservedResourceMemoryManager::UploadData(ComPtr< ID3D12Device> creationdevice,DX12Commandlist& uploadcmdlist)
{
	if (m_subrestoupload.size() == 0)
	{
		//nothing to do if nothing to upload.
		return;
	}

	if(m_restomanage->GetCurrentResourceState()!= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST)
	{
		D3D12_RESOURCE_BARRIER barrier = m_restomanage->TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);
		uploadcmdlist->ResourceBarrier(1, &barrier);
	}
	for (unsigned i = 0; i < m_subrestoupload.size(); i++)
	{
		UINT subresidx = m_subrestoupload[i];
		//create un upload buffer
		UINT64 uploadbuffersize = GetRequiredIntermediateSize(m_restomanage->GetResource().Get(), subresidx, 1);
		DX12Buffer* uploadbuffer = CreateUploadBuffer(creationdevice, uploadbuffersize);
		UpdateSubresources(uploadcmdlist.GetcmdList(), m_restomanage->GetResource().Get(),uploadbuffer->GetResource().Get(), 0, subresidx, 1, &m_reservedresourcesubresdata[subresidx]);
	}
	m_subrestoupload.clear();
	if (m_restomanage->GetCurrentResourceState() != D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	{
		D3D12_RESOURCE_BARRIER barrier = m_restomanage->TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		uploadcmdlist->ResourceBarrier(1, &barrier);
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
		if (subresinfo.isMapped)
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

	//unbind all heaps not needed & delete heaps
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
		//mark for upload if needed.
		if (m_allowuploadonmapping)
		{
			m_subrestoupload.push_back(targetsubresidx);
		}
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
void DX12ReservedResourceMemoryManager::ClearMip(DX12Commandlist& cmdlist, unsigned mipindex, float* clearcolour)
{
	if (m_uavheap.GetDescHeap() == nullptr)
	{
		//not using uavs
		return;
	}
	//bind uav heap
	ID3D12DescriptorHeap* heapstoset[] = { m_uavheap.GetDescHeap() };
	cmdlist->SetDescriptorHeaps(1, heapstoset);
	if (m_restomanage->GetCurrentResourceState() != D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		D3D12_RESOURCE_BARRIER barrier = m_restomanage->TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		cmdlist->ResourceBarrier(1, &barrier);
	}
	D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle = m_uavheap.GetGPUHandleOffseted(mipindex);
	D3D12_CPU_DESCRIPTOR_HANDLE cpuhandle = m_uavheapupload.GetCPUHandleOffseted(mipindex);
	cmdlist->ClearUnorderedAccessViewFloat(gpuhandle, cpuhandle, m_restomanage->GetResource().Get(), clearcolour, 0, nullptr);
	if (m_restomanage->GetCurrentResourceState() != D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	{
		D3D12_RESOURCE_BARRIER barrier = m_restomanage->TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdlist->ResourceBarrier(1, &barrier);
	}
}

bool DX12ReservedResourceMemoryManager::IsMemoryBound(UINT subresourceindex)
{
	assert(subresourceindex < m_subresourceinfo.size());
	return m_subresourceinfo[subresourceindex].isMapped;
}
unsigned DX12ReservedResourceMemoryManager::GetMostDetailedMappedMipIndex()
{
	//iterate through each subres data(from most detailed to least detailed mip,choose the first one mapped
	for (size_t i = 0; i < m_subresourceinfo.size(); i++)
	{
		if (m_subresourceinfo[i].isMapped)
		{
			return i;
		}
	}
}

DX12Buffer* DX12ReservedResourceMemoryManager::CreateUploadBuffer(ComPtr< ID3D12Device> creationdevice,UINT64 size)
{
	DX12Buffer* anuplodbuffer = new DX12Buffer();
	DX12ResourceCreationProperties uploadbufferprops;
	DX12Buffer::InitResourceCreationProperties(uploadbufferprops);
	uploadbufferprops.resdesc.Width = size;
	uploadbufferprops.resheapprop.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	uploadbufferprops.resinitialstate = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
	anuplodbuffer->Init(creationdevice, uploadbufferprops, ResourceCreationMode::COMMITED);
	m_uploadbuffers.push_back(anuplodbuffer);
	return anuplodbuffer;
}