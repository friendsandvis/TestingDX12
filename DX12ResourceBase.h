#pragma once
#include"DXUtils.h"
enum ResourceCreationMode
{
	COMMITED,
	PLACED,
	RESERVED
};

struct DX12ResourceCreationProperties
{
	D3D12_RESOURCE_DESC resdesc;
	D3D12_HEAP_PROPERTIES resheapprop;
	D3D12_HEAP_FLAGS resheapflags;
	D3D12_RESOURCE_STATES resinitialstate;
	D3D12_CLEAR_VALUE optimizedclearvalue;
	bool useclearvalue = false;


};


class DX12ResourceBase
{
public:
	inline ResourceCreationMode GetResourcecreationMode() { return m_rescreationmode; }
	inline ComPtr<ID3D12Resource> GetResource() { return m_resource; }
	inline void SetName(LPCWSTR name) { m_resource->SetName(name); }
	bool IsResourceState(D3D12_RESOURCE_STATES resstate) { return(m_currentresstate == resstate); }
	void SetResState(D3D12_RESOURCE_STATES resstate) { m_currentresstate = resstate; }
	inline D3D12_RESOURCE_STATES GetCurrentResourceState() { return m_currentresstate; }
	D3D12_RESOURCE_BARRIER TransitionResState(D3D12_RESOURCE_STATES targetstate);
	bool IsResState(D3D12_RESOURCE_STATES targetstate) { return(m_currentresstate == targetstate); }

protected:
	ResourceCreationMode m_rescreationmode;
	ComPtr<ID3D12Resource> m_resource;
	D3D12_RESOURCE_STATES m_currentresstate;
private:

};