#pragma once
#include"DXUtils.h"

enum ResourceCreationMode
{
	COMMITED,
	PLACED,
	RESERVED
};

class DX12ResourceBase
{
public:
	inline ResourceCreationMode GetResourcecreationMode() { return m_rescreationmode; }
	inline ComPtr<ID3D12Resource> GetResource() { return m_resource; }
	inline void SetName(LPCWSTR name) { m_resource->SetName(name); }
	void SetResState(D3D12_RESOURCE_STATES resstate) { m_currentresstate = resstate; }
	D3D12_RESOURCE_BARRIER TransitionResState(D3D12_RESOURCE_STATES targetstate);
	
protected:
	ResourceCreationMode m_rescreationmode;
	ComPtr<ID3D12Resource> m_resource;
	D3D12_RESOURCE_STATES m_currentresstate;
private:
	
};

struct DX12ResourceCreationProperties
{
	D3D12_RESOURCE_DESC resdesc;
	D3D12_HEAP_PROPERTIES resheapprop;
	D3D12_HEAP_FLAGS resheapflags;
	D3D12_RESOURCE_STATES resinitialstate;
	D3D12_CLEAR_VALUE optimizedclearvalue;
	bool useclearvalue=false;


};

class DX12Resource:public DX12ResourceBase
{
public:
	void Init(ComPtr< ID3D12Device> creationdevice, DX12ResourceCreationProperties resprops, ResourceCreationMode creationmode);
	void CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle);
	void CreateUAV(ComPtr< ID3D12Device> creationdevice,D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle);
	//initialize the resource creation properties with defaults to prevent missing any params while manual init
	static void InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops);

protected:
	DX12ResourceCreationProperties m_creationproperties;
};

//the subresource is designed to treat mips as sub resource other elements(like depth/array) not considered in subresource calculations.
class DX12ReservedResource :public DX12ResourceBase
{
public:
	void Init(ComPtr< ID3D12Device> creationdevice, DX12ResourceCreationProperties resprops);
	void CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle);
	void CreateUAV(ComPtr< ID3D12Device> creationdevice, D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle);
	const D3D12_PACKED_MIP_INFO& GetPackedMipInfo() { return m_packedmipinfo;}
	const vector<D3D12_SUBRESOURCE_TILING> GetSubResourceTilingInfo() {return m_subresourcetiling;}
	DX12ReservedResource();
	~DX12ReservedResource();
	// initialize the resource creation properties with defaults to prevent missing any params while manual init
		static void InitResourceCreationProperties(DX12ResourceCreationProperties & rescreationprops);

protected:
	DX12ResourceCreationProperties m_creationproperties;
	D3D12_PACKED_MIP_INFO m_packedmipinfo;
	vector<D3D12_SUBRESOURCE_TILING> m_subresourcetiling;
};

class DX12ReservedresourcePhysicalMemoryManager
{
public:
	//initialize reservedresourcememory manager(prepare everything needed for mapping physical memory to reserved resource)
	void Init(ComPtr< ID3D12Device> creationdevice, DX12ReservedResource* resourcetomanage);
	void UpdateReservedresourcePhysicalMemoryMappings(ComPtr<ID3D12CommandQueue>commandqueue);
	DX12ReservedresourcePhysicalMemoryManager();

	~DX12ReservedresourcePhysicalMemoryManager();
private:
	DX12ReservedResource* m_reservedres;
	vector<ComPtr<ID3D12Heap>>m_heaps;

	//represents 1 subresource info in reserved resource as needed for memory management(technique taken from dx12 sample)
	struct SubResouceInfo
	{
		//1 heap per subresource so offset always 0
		UINT heapindex;
		D3D12_TILE_REGION_SIZE tileregionsize;
		D3D12_TILED_RESOURCE_COORDINATE coordinates;
		bool isMapped = false;//is the subresource mapped to the reserved resource

	};

	//needed for per subresource memory management in reserved resource
	vector<SubResouceInfo> m_subresourceinfo;

	//utility function to initialize default heaps
	void InitHeap(size_t indexinheapvector, ComPtr< ID3D12Device> creationdevice, UINT64 heapsize);
};




