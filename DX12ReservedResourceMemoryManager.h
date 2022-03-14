#pragma once
#include"DX12Resource.h"


struct SubResouceInfo
{
	//1 heap per subresource so offset always 0
	UINT heapindex;

	D3D12_TILE_REGION_SIZE tileregionsize;
	D3D12_TILED_RESOURCE_COORDINATE coordinates;
	bool isMapped = false;//is the subresource mapped to the reserved resource
	bool ispacked = false;



};

class DX12ReservedResourceMemoryManager
{
public:
	DX12ReservedResourceMemoryManager();
	~DX12ReservedResourceMemoryManager();
	inline size_t GetSubResCount() { return m_subresourceinfo.size(); }
	void Init(DX12ReservedResource* reservedresourcetomanage);
	//updates the physical memory mapping for the reserved resource(queue needed for updating the mappings and device for creating new heaps if needed)
	void Update(ComPtr<ID3D12CommandQueue>commandqueue, ComPtr< ID3D12Device> creationdevice);
	void BindMemory(UINT subresourceindex);
	void UnbindMemory(UINT subresourceindex);
	//returns whether memory for the subresoource is bound or not
	bool IsMemoryBound(UINT subresourceindex);
	DX12ReservedResource* GetReservedresource() { return m_restomanage; }
private:
	DX12ReservedResource* m_restomanage;
	//information about all the subresources in the reserved resource in the form needed for memory management
	vector<SubResouceInfo> m_subresourceinfo;
	//heaps needed for backing reserved resource
	vector<ComPtr<ID3D12Heap>>m_heaps;
	//used to mark the subresources that need to be are dirty(binded/unbined).
	vector<UINT> m_dirtysubres;
	

	//utility function to initialize default heaps
	void InitHeap(size_t indexinheapvector, ComPtr< ID3D12Device> creationdevice, UINT64 heapsize);
	//utility function to delete a heap
	void DeleteHeap(size_t heapindex);
};

