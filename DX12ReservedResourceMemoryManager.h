#pragma once
#include"DX12ReservedResource.h"
#include"DX12CommandList.h"
#include"DX12Buffer.h"
#include"DX12DESCHEAP.h"


struct SubResouceInfo
{
	//1 heap per subresource so offset always 0
	UINT heapindex;

	D3D12_TILE_REGION_SIZE tileregionsize;
	D3D12_TILED_RESOURCE_COORDINATE coordinates;
	bool isMapped = false;//is the subresource mapped to the reserved resource
	bool ispacked = false;
	bool isunmapable = true;//states if the subres can be unmaped



};


struct ManagedHeap
{
	ComPtr<ID3D12Heap> heap;
	size_t usagecount;//keeps of of usage for this heap(if 0 then not being used by anyone,can be null).
	ManagedHeap()
	{
		heap = ComPtr<ID3D12Heap>();
		usagecount = 0;
	}
	//returns true if delete success
	bool Delete()
	{
		//reduce usage count and release if usage is 0
		--usagecount;
		if (usagecount == 0)
		{
			heap->Release();
			heap = ComPtr<ID3D12Heap>();
			assert(heap.Get() == nullptr);
			return true;
		}
		return false;
	}
};
class DX12ReservedResourceMemoryManager
{
public:
	DX12ReservedResourceMemoryManager();
	~DX12ReservedResourceMemoryManager();
	inline size_t GetSubResCount() { return m_subresourceinfo.size(); }
	void Init(ComPtr< ID3D12Device> creationdevice,DX12ReservedResource* reservedresourcetomanage, bool inituav=false);
	//updates the physical memory mapping for the reserved resource(queue needed for updating the mappings and device for creating new heaps if needed)
	void Update(ComPtr<ID3D12CommandQueue>commandqueue, ComPtr< ID3D12Device> creationdevice);
	
	void RequestUploadData(UINT subresindextoupload);
	void UploadData(ComPtr< ID3D12Device> creationdevice,DX12Commandlist& uploadcmdlist);
	void ClearUAV(DX12Commandlist& uploadcmdlist);
	void BindMemory(UINT subresourceindex,bool makeunmapable=false);
	void BindMemory2(UINT subresourceindex);
	//returns if the unbind operation was issued for the requested resourceindex or not.
	bool UnbindMemory(UINT subresourceindex);
	//used to clear uav for a reserved resource mip if uav allowed
	void ClearMip(DX12Commandlist& cmdlist,unsigned mipindex, float* clearcolour);
	//allows uploading a mip's data upon binding it's memory in reserved resource
	void AllowUploadOnMapping(bool allow)
	{
		m_allowuploadonmapping=allow;
	}
	
	//returns whether memory for the subresoource is bound or not
	bool IsMemoryBound(UINT subresourceindex);
	bool IsPacked(UINT subresourceindex);
	unsigned GetMostDetailedMappedMipIndex();
	DX12ReservedResource* GetReservedresource() { return m_restomanage; }
private:
	
	DX12ReservedResource* m_restomanage;
	//information about all the subresources in the reserved resource in the form needed for memory management
	vector<SubResouceInfo> m_subresourceinfo;
	//heaps needed for backing reserved resource
	vector<ManagedHeap>m_heaps;
	//used to mark the subresources that need to be are dirty(binded/unbined).
	vector<UINT> m_dirtysubres;
	//a vector holding intermidiate buffer(used for upload).
	vector<D3D12_SUBRESOURCE_DATA> m_reservedresourcesubresdata;
	DX12Buffer m_uploadbuffer;
	DX12DESCHEAP m_uavheap,m_uavheapupload;
	vector<UINT> m_subrestoupload;
	bool m_allowuploadonmapping;
	vector<DX12Buffer*> m_uploadbuffers;
	

	//utility function to initialize default heaps
	void InitHeap(size_t indexinheapvector, ComPtr< ID3D12Device> creationdevice, UINT64 heapsize);
	//utility function to delete a heap(return true if delete success else false)
	bool DeleteHeap(size_t heapindex);
	//utility function to create upload buffer for data copy
	DX12Buffer* CreateUploadBuffer(ComPtr< ID3D12Device> creationdevice,UINT64 size);
};

