#pragma once
#include"DX12Resource.h"

struct TextureCreationProperties
{
	D3D12_RESOURCE_DESC texdesc;
	D3D12_RESOURCE_STATES initstate;
	D3D12_CLEAR_VALUE clearvalue;
	D3D12_HEAP_PROPERTIES heapprop;
	D3D12_HEAP_FLAGS heapflags;
};

//a simple dx12 texture which does not represent any specialized use of texture 
class DX12TextureSimple:public DX12Resource
{
public:
	
	DX12TextureSimple();
	~DX12TextureSimple();

};

