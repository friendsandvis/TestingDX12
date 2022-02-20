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

protected:
	DX12ResourceCreationProperties m_creationproperties;
};
