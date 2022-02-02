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
	void SetResState(D3D12_RESOURCE_STATES resstate) { m_currentresstate = resstate; }
	D3D12_RESOURCE_BARRIER TransitionResState(D3D12_RESOURCE_STATES targetstate);
protected:
	ResourceCreationMode m_rescreationmode;
	ComPtr<ID3D12Resource> m_resource;
	D3D12_RESOURCE_STATES m_currentresstate;
private:
	
};
