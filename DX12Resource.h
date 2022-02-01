#pragma once
#include"DXUtils.h"

enum ResourceCreationMode
{
	COMMITED,
	PLACED,
	RESERVED
};

class DX12Resource
{
public:
	inline ResourceCreationMode GetResourcecreationMode() { return m_rescreationmode; }
	inline ComPtr<ID3D12Resource> GetResource() { return m_resource; }
	inline void SetName(LPCWSTR name) { m_resource->SetName(name); }
protected:
	ResourceCreationMode m_rescreationmode;
	D3D12_RESOURCE_STATES m_currentstate;
	ComPtr<ID3D12Resource> m_resource;
private:
	
};
