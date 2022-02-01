#pragma once
#include"DX12Resource.h"

struct BufferCreationProperties
{
	D3D12_RESOURCE_DESC resdesc;
	D3D12_HEAP_PROPERTIES resheapprop;
	D3D12_HEAP_FLAGS resheapflags;
	D3D12_RESOURCE_STATES resinitialstate;
	

};

struct BufferMapParams
{
	UINT subresource;
	D3D12_RANGE range;
};

class DX12Buffer:public DX12Resource
{
	
public:
	void* Map(BufferMapParams readparams);
	void UnMap(BufferMapParams writeparams);
	void Init(ComPtr< ID3D12Device> creationdevice, BufferCreationProperties bufferresprop, ResourceCreationMode creationmode);
	inline UINT64 GetSize() { return m_properties.resdesc.Width; }
private:
	void CreateBuffer(ComPtr< ID3D12Device> creationdevice);

protected:
	BufferCreationProperties m_properties;
	bool m_is_uploadtype;
};