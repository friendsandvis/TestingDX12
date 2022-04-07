#pragma once
#include"DXUtils.h"
#include"DX12CommandList.h"
#include"DX12DESCHEAP.h"
#include"DX12ResourceBase.h"





class DX12Resource:public DX12ResourceBase
{
public:
	void Init(ComPtr< ID3D12Device> creationdevice, DX12ResourceCreationProperties resprops, ResourceCreationMode creationmode);
	void CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle);
	void CreateUAV(ComPtr< ID3D12Device> creationdevice,D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle);
	void CreateDSV(ComPtr< ID3D12Device> creationdevice, D3D12_DEPTH_STENCIL_VIEW_DESC dsvdesc, D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle);
	//initialize the resource creation properties with defaults to prevent missing any params while manual init
	static void InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops);

protected:
	DX12ResourceCreationProperties m_creationproperties;
};





