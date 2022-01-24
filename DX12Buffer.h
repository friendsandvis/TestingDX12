#pragma once
#include"DX12Resource.h"

struct BufferCreationProperties
{
	CD3DX12_RESOURCE_DESC resdesc;
	D3D12_HEAP_PROPERTIES resheapprop;
	D3D12_HEAP_FLAGS resheapflags;
	D3D12_RESOURCE_STATES resinitialstate;
	

};

class DX12BUFFER:public DX12Resource
{
	
public:
	
	void Init(ComPtr< ID3D12Device> creationdevice, BufferCreationProperties bufferresprop, ResourceCreationMode creationmode);

private:
	void CreateBuffer(ComPtr< ID3D12Device> creationdevice);

protected:
	BufferCreationProperties m_properties;
};