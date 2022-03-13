#pragma once
#include"DX12Resource.h"


struct BufferMapParams
{
	UINT subresource;
	D3D12_RANGE range;
};

class DX12Buffer:public DX12Resource
{
	
public:
	static void InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops);
	void* Map(BufferMapParams readparams);
	void UnMap(BufferMapParams writeparams);
	inline UINT64 GetSize() { return m_creationproperties.resdesc.Width; }


};